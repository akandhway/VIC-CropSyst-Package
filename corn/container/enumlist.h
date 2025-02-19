
// This class is probably obsolete, because Items can now be keyed by int32 or string

#ifndef enumlistH
#define enumlistH
#ifndef bi_listH
#  include "corn/container/bilist.h"
#endif
//    An enumeration list is similar to an
//          but the key is an unsigned integer;
namespace CORN {
//______________________________________________________________________________
//   Now based on bidirectional list so association lists can be used in RUI controls
// Items appended to the enumeration list are kept in the order added.
class Enumeration_list
: public Bidirectional_list
{
 public:
   Enumeration_list();                                                           //160616_051010
   Enumeration_list(bool unique_keys);                                           //160616_051010
      //121112 NYI should have the option to have unique keys.
      // This would probably be common to all container classes.
   virtual ~Enumeration_list();                                                  //081125
   virtual Item *find(nat32 search_key)                                   const; //020319
      /**<\fn \return the association with the matching key
         \return 0 if no matching association. */
   virtual nat32 find_index(nat32 search_key)                             const; //130125
      /**<\fn \return the index (0 based position) of the item matching the search key.
         \return npos if no matching association.
      */

      // Note, currently this class has no restriction on mutiple items having
      // the same key value.  The find methods return the first occurance.

   virtual nat32 remove_all_matching(nat32 search_key)            modification_; //020319
      /**<\fn Remove removes all matching entries from the list
         and deletes the associated data!
         \Return number of matching deletionions
            //140716 true if there was a matching deletion.
      **/
};
//_class_Enumeration_list___________________________________________2001-10-31_/
}//_namespace CORN_____________________________________________________________/
#endif
// enumlist.h

