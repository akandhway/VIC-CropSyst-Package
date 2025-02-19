#ifndef bilistH
#define bilistH
#ifndef unilistH
#  include "corn/container/unilist.h"
#endif
namespace CORN {
class Bidirectional_list; // Forward declaration for self
//______________________________________________________________________________
class Bidirectional_list : public Unidirectional_list
{
public:  // public only for BC5 which cannot see the protected node!
   class Node : public Unidirectional_list::Node
   {friend class Bidirectional_list;
   public:
      Node *prev;
   public:
      Node(Item *i_data);
      virtual ~Node();                                                           //021118
   };
   //___________________________________________________________________________
public:
   class Iterator                                                                //060233
   : public Unidirectional_list::Iterator
   {public: // structors
      Iterator(Bidirectional_list      &container_);
      virtual ~Iterator();
   public: //
      virtual Item *prev();
      virtual bool has_predecessor()                               affirmation_; //180526
    public:
      virtual Item *detach_current() ;                                           //180418
   };
   friend class Unidirectional_list::Iterator;
   //___________________________________________________________________________
protected:
   mutable bool moving_forward;                                                  //011031
protected:
   void append_node(Node *node);                                                 //980206
   void fixup_pointers();                                                        //980206
public:
   Bidirectional_list(bool unique_keys_=false);                                  //160615
   inline virtual ~Bidirectional_list() {}                                       //151204
   virtual Container::Iterator *iterator()                                const; //060322
   virtual bool remove(Item *remove_element);
      // Deletes the remove_element.  If remove_element is 0 it does nothing.

      // NYI return bool  true if it was removed 170701

   virtual Item *append(Item *new_element)                       appropriation_;
   virtual Item *prepend(Item *new_element)                      appropriation_;
   virtual Item *add_sorted                                                      //970928
      (Item *new_element, Ordering ordering = ASCENDING)         appropriation_; //971211
   virtual Item *pop_at_head();
   virtual Item *find_matching_reverse
      (const Item &item_to_match,bool from_end = true)                    const; //030117
   virtual nat32 write(std::ostream &strm)                        modification_; //150729
      // Differs from unilist write in that if bilist general direction
      // is reverse, the list will be written backwards.
   // Functions unique to bidirectional list
   virtual void swap_elements(Node *element1,Node *element2);
   virtual void sort(Ordering ordering = ASCENDING);
};
//___________________________________________________class:Bidirectional_list__/
}//_namespace CORN_____________________________________________________________/
#endif

