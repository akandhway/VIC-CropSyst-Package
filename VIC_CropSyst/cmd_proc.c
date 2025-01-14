#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vicNl.h>
#ifdef __BCPLUSPLUS__
//190131 RLN BC++ doesn't have get opt
extern int getopt(int argc, char *argv[], char * optstring);
#else
#include <unistd.h> //LML 140812 for getopt()
#endif

static char vcid[] = "$Id: cmd_proc.c,v 4.3.2.2 2012/02/05 00:15:44 vicadmin Exp $";

filenames_struct cmd_proc(int argc, char *argv[]) 
/**********************************************************************
  cmd_proc                  Keith Cherkauer                1997

  This routine checks the command line for valid program options.  If
  no options are found, or an invalid combination of them appear, the
  routine calls usage() to print the model usage to the screen, before
  exiting execution.

  Modifications:
  11-18-98  Added comment block to cmd_proc() and fixed routine so
            that it will exit if global command file is not defined
            using the "-g" flag.                                KAC
  2003-Oct-03 Added -v option to display version information.		TJB
  2012-Jan-16 Removed LINK_DEBUG code					BN
**********************************************************************/
{
  extern option_struct options;
  //LML 140812 commended out extern int getopt();
  extern char *optarg;
  extern char *optstring;

  filenames_struct names;
  int              optchar;
  char             GLOBAL_SET;
  
  if(argc==1) {
    usage(argv[0]);
    exit(1);
  }
  
  GLOBAL_SET = FALSE;
#ifdef SEPERATE_SOIL_PARAMETER_AND_CROP_OUTPUT
  strcpy(names.soil, "");                                                        //171005LML
  strcpy(names.result_dir, "");                                                  //171005LML
  strcpy(names.f_path_pfx[0], "");                                               //171006LML
  strcpy(names.f_path_pfx[1], "");                                               //171006LML
#endif

  while((optchar = getopt(argc, argv, optstring)) != EOF) {
    switch((char)optchar) {
    case 'v':
      /** Version information **/
      display_current_settings(DISP_VERSION,(filenames_struct*)NULL);
      exit(0);
      break;
    case 'o':
      /** Compile-time options information **/
      display_current_settings(DISP_COMPILE_TIME,(filenames_struct*)NULL);
      exit(0);
      break;
    case 'g':
      /** Global Parameters File **/
      strcpy(names.global, optarg);
      GLOBAL_SET = TRUE;
      break;
#ifdef SEPERATE_SOIL_PARAMETER_AND_CROP_OUTPUT
    case 's':
      /** Soil Parameter File **/
      strcpy(names.soil, optarg);
      break;
    case 'c':
      /** Soil Parameter File **/
      strcpy(names.result_dir, optarg);
      break;
    case 'f':
      /** Soil Parameter File **/
      strcpy(names.f_path_pfx[0], optarg);
      break;
#endif
    default:
      /** Print Usage if Invalid Command Line Arguments **/
      usage(argv[0]);
      exit(1);
      break;
    }
  }

  if(!GLOBAL_SET) {
    fprintf(stderr,"ERROR: Must set global control file using the '-g' flag\n");
    usage(argv[0]);
    exit(1);
  }

  return names;
}


void usage(char *temp)
/**********************************************************************
	usage		Keith Cherkauer		May 27, 1996

  This routine prints out usage details.

**********************************************************************/
{
  fprintf(stderr,"Usage: %s [-v | -o | -g<global_parameter_file>]\n",temp);
  fprintf(stderr,"  v: display version information\n");
  fprintf(stderr,"  o: display compile-time options settings (set in user_def.h)\n");
  fprintf(stderr,"  g: read model parameters from <global_parameter_file>.\n");
  fprintf(stderr,"       <global_parameter_file> is a file that contains all needed model\n");
  fprintf(stderr,"       parameters as well as model option flags, and the names and\n");
  fprintf(stderr,"       locations of all other files.\n");
#ifdef SEPERATE_SOIL_PARAMETER_AND_CROP_OUTPUT
  fprintf(stderr,"  s: <soil_parameter_file> for this run.\n");
  fprintf(stderr,"  c: <output_directory> for this run.\n");
  fprintf(stderr,"  f: <forcingdata_directory_and_prefix> for this run.\n");
#endif
}
