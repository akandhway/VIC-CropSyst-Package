#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include "file_system_engine_Qt.h"
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/seclusion.h"

namespace CORN { namespace OS {
//______________________________________________________________________________
File_system_engine_Qt::File_system_engine_Qt()
{}
//__________________________________________________________________2015-03-20_/
nat32 File_system_engine_Qt::list_names_in
(const Directory_name & directory_name
,CORN::Text_list *file_name_list_unqual
,CORN::Text_list *directory_name_list_unqual)                              const
{  QString dirName;
   dirName = QString::fromWCharArray(directory_name.w_str());
   QDir dir(dirName);
   if (dir.exists(dirName))
   {  Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
      {  std::wstring unqualified_name(info.fileName().toStdWString());
         if (info.isDir() && directory_name_list_unqual)
            directory_name_list_unqual->add_wstring(unqualified_name);
         else  // its a normal file name
            if (file_name_list_unqual)
               file_name_list_unqual->add_wstring(unqualified_name);
      } // Q_FOREACH
   }
   return file_name_list_unqual      ? file_name_list_unqual->count()      : 0
        + directory_name_list_unqual ? directory_name_list_unqual->count() : 0;
}
//_list_names_in____________________________________________________2016-08-29_/
nat32 File_system_engine_Qt::list_entry_names_in_deprecated
(const Directory_name & directory_name
,CORN::Text_list *file_name_list_unqual
,CORN::Text_list *directory_name_list_unqual
,const wchar_t *name_wild_card
,const wchar_t *extension_filter
,const CORN::Text_list *exclude_directories)                               const
{  nat32 file_list_count = 0;
   nat32 dir_list_count = 0;
   QString dirName;
   dirName = QString::fromWCharArray(directory_name.w_str());
   QDir dir(dirName);
   if (dir.exists(dirName))
   {
      Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
      {
         bool extension_filter_matches = (extension_filter == 0);
         if (!extension_filter_matches)
         {
            std::string extension_filter_string; CORN::UnicodeZ_to_string(extension_filter,extension_filter_string);
            /*
            QString extension_filter_Qstring(extension_filter_string.c_str()); //(L"test2");
            // Not sure why this doesnt do anything: extension_filter_Qstring.fromWCharArray(extension_filter);
            extension_filter_matches =
                  info.suffix() == extension_filter_Qstring;
           */
            std::string info_suffix(info.suffix().toStdString());
            extension_filter_matches = info_suffix == extension_filter_string;
         }
         if (extension_filter_matches)
         {  // list only directories that have the specified extension
            bool wild_card_matches = (name_wild_card == 0);
            if (!wild_card_matches)
            {
               QString name_wild_card_Qstring;
               name_wild_card_Qstring.fromWCharArray(name_wild_card);
               QRegExp regexp
                  (name_wild_card_Qstring
                  #ifdef __linux__
                  ,Qt::CaseSensitive
                  #else
                  ,Qt::CaseInsensitive
                  #endif
                  ,QRegExp::WildcardUnix);
               wild_card_matches = regexp.exactMatch(info.completeBaseName());
            }
            if (wild_card_matches)
            {
               if (info.isDir())
               {
                  if (directory_name_list_unqual)
                  {
                      directory_name_list_unqual->add_wstring(info.fileName().toStdWString());
                      dir_list_count++;
                  }
               } else  // its a normal file name
               {
                  if (file_name_list_unqual)
                  {
                     file_name_list_unqual->add_wstring(info.fileName().toStdWString());
                     file_list_count++;
                  }
               }
            } //matches
         } // extension filtered
      } // Q_FOREACH
   }
   return file_list_count + dir_list_count;
}
//_list_entry_names_in_deprecated___________________________________2015-12-12_/
bool File_system_engine_Qt::matches_any_in                                       //151212
(const std::wstring &name_with_ext
,const CORN::Text_list &wild_card_patterns_to_compare
,bool case_insensitive)                                             affirmation_
{   bool matches = false;
   FIND_FOR_EACH_IN(matching_pattern,pattern,CORN::Item_string
      ,wild_card_patterns_to_compare,true,each_WCP)
   {
      QString name_wild_card_Qstring;
      name_wild_card_Qstring.fromStdString(*pattern);
      QRegExp regexp
               (name_wild_card_Qstring
               ,case_insensitive ? Qt::CaseInsensitive :Qt::CaseSensitive
               ,QRegExp::WildcardUnix);
      matches = regexp.exactMatch(name_wild_card_Qstring);
      if (matches) matching_pattern = pattern;
   } FOR_EACH_END(each_WCP)
   return matches;
}
//_matches_any_in___________________________________________________2015-12-12_/
std::wstring *File_system_engine_Qt::render_environment_variable_value
(const std::wstring &envvar_wstring)                                  rendition_
      // Must be implemented for specific OS in derived file_system_engine class.
{
   std::string envvar; CORN::wstring_to_string(envvar_wstring,envvar);
   char *envvar_paths_raw = getenv(envvar.c_str());
   std::wstring *envvar_paths
      =  envvar_paths_raw ? new std::wstring : 0;
   if (envvar_paths)
   {  CORN::ASCIIZ_to_wstring(envvar_paths_raw,*envvar_paths);
   }
   return envvar_paths; // may be 0
}
//_render_environment_variable_value________________________________2018-08-06_/
}}//_namespace_CORN_OS_________________________________________________________/


