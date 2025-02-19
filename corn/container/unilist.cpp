#include "corn/container/unilist.h"
namespace CORN {
//______________________________________________________________________________
Unidirectional_list::Unidirectional_list(bool unique_keys_)
:Container(unique_keys_)
,head(0)
,tail(0)
{ }
//_Unidirectional_list:constructor__________________________________2015-08-19_/
Unidirectional_list::~Unidirectional_list()
{  delete_all();}
//_Unidirectional_list:destructor___________________________________1997-03-30_/
void Unidirectional_list::append_node(Node *node)
{  if (node)
   {  if (!head) head = node;                                                    //980508
      if (tail)  tail->next = node;
      tail = node;
      node->next = 0;
      set_modified(true);                                                        //060321
} }
//_append_node______________________________________________________1998-02-06_/
void Unidirectional_list::prepend_node(Node *node)                               //110821
{  if (!tail)
   {  tail = node; node->next = 0; }
   node->next = head;
   head = node;
   set_modified(true);                                                           //060321
}
//_prepend_node_____________________________________________________1998-02-06_/
Item * Unidirectional_list::append(Item *new_element)
{  Item *appropriatable_element = delete_unappropriatable(new_element);          //170522_161024
   if (appropriatable_element)                                                   //170522_150818
      append_node(new Node(new_element));
   return appropriatable_element;                                                //170522_150818_980511
}
//_append______________________________________________________________________/
Item * Unidirectional_list::prepend(Item *new_element)
{  Item *appropriatable_element = delete_unappropriatable(new_element);          //170928
   if (appropriatable_element)                                                   //170928
      prepend_node(new Node(new_element));                                       //170928
   return appropriatable_element;                                                //170928
}
//_prepend_____________________________________________________________________/
Unidirectional_list::Node *Unidirectional_list::find_node_with_element
(Item *element) const
{   for (Node *node = head; node; node = node->next)
       if (node->get() == element)
          return node;
    return 0;
}
//_find_node_with_element___________________________________________1998-02-06_/
Item *Unidirectional_list::insert_before(Item *new_element,Item *before_element)
{  Item *return_element = new_element;                                           //150818
   if (new_element)                                                              //150818
   {  Item *existing_element = unique_keys ? find_comparable(*new_element) : 0;
      if (existing_element)
      {  return_element = existing_element;
         delete new_element;
      } else
      {  Node *node = new Node(new_element);                                     //980206
         Node *before_node = find_node_with_element(before_element);
         Node *prev = find_prev_of(before_element);
         if ((before_node == head) || !prev)
            prepend_node(node);                                                  //110821
         else
         {  prev->next = node;
            node->next = before_node;
            set_modified(true);                                                  //060321
   }  }  }
   return return_element;
}
//_insert_before_______________________________________________________________/
Item *Unidirectional_list::pop_tail()
{   Item *return_element = 0;                                                    //070808
    if (tail)
    {  return_element = tail->get();
       tail->set(0);
       Node *new_tail = (find_prev_of(tail));                                    //980209
       if (tail == head)                                                         //070915
          head = 0;                                                              //070915
       delete tail;                                                              //980206
       if (new_tail) new_tail->next = 0;
       tail = new_tail;                                                          //070808
       set_modified(true);                                                       //060321
    }
    return return_element;                                                       //070808
}
//_pop_tail_________________________________________________________________/
Item *Unidirectional_list::pop_head()
{ if (!head) return(0);
  Node *head_node = head;                                                        //980206
  Item *hold_element = head->get();
  head = head->next;
  head_node->next = 0;
  head_node->set(0);                                                             //980209
  if (!head) tail = 0;
  delete head_node;                                                              //980206
  set_modified(true);                                                            //060321
  return hold_element;
}
//_pop_head_________________________________________________________________/
Item *Unidirectional_list::pop_first()
{ return pop_head(); }
//_pop_first________________________________________________________1999-11-22_/
Item *Unidirectional_list::pop_last()
{ return pop_tail(); }
//_pop_last_________________________________________________________2017-12-04_/
nat32 Unidirectional_list::delete_all(Item::Filter filter)
{  nat32 count_deleted = 0;
   if (head)                                                                     //040319
   {  Unidirectional_list not_deleted;                                           //040318
      while (head)
      {  Item *del_el = pop_head();
         if (del_el)
         {  if (del_el->matches_filter(filter))                                  //040318
            {  count_deleted += 1;                                               //040107
               delete del_el;
            } else not_deleted.append(del_el);                                   //040318
      }  }
      if (not_deleted.count())                                                   //040319
         transfer_all_from(not_deleted);                                         //040318
   }
   return count_deleted;                                                         //040107
}
//_delete_all__________________________________________________________________/
Item *Unidirectional_list::get_at(nat32 index)                             const
{ // elements are 0 indexed
  // index >= the number of elements then NULL is returned
  nat32 count = 0;
  for (Node *node = head; node; node = node->next)                                //010917
  { if (count == index)                                                           //991216
      return node->get();
    count++;
  }
  return(0);
}
//_get_at___________________________________________________________1999-12-16_/
nat32 Unidirectional_list::count() const
{ nat32 cnt = 0;
  for (Node *local_curr = head; local_curr; local_curr = local_curr->next)
      cnt ++;
  return cnt;
}
//_count____________________________________________________________1999-01-04_/
Unidirectional_list::Node *Unidirectional_list::find_prev_of(Item *element) const
{  for (Node *srch = head; srch; srch = srch->next)
      if (srch->next && (srch->next->get() == element)) return srch;
   return 0;
}
//_find_prev_of_____________________________________________________1998-04-04_/
Unidirectional_list::Node *Unidirectional_list::find_prev_of(Node *node) const
{  for (Node *srch = head; srch; srch = srch->next)
      if (srch->next == node) return srch;
   return 0;
}
//_find_prev_of_____________________________________________________1997-09-30_/
Container::Iterator *Unidirectional_list::iterator()                       const
{  Unidirectional_list &this_container = (Unidirectional_list &)*this;
   Unidirectional_list::Iterator *itor = new Unidirectional_list::Iterator(this_container);
   return itor;
}
//_iterator_________________________________________________________2003-06-03_/
Unidirectional_list::Iterator::Iterator(Unidirectional_list &i_container)
: Container::Iterator()
, container(i_container)
, curr_node(container.head)                                                      //050322
, detached_head(false)                                                           //161212
{}
//_Unidirectional_list:Iterator:constructor_________________________2003-06-03_/
Unidirectional_list::Iterator::~Iterator()
{}
//_Unidirectional_list:Iterator:destructor_____________________________________/
bool Unidirectional_list::is_empty()                                       const
{ return (head == 0); }
//_is_empty_________________________________________________________2004-01-07_/
Item *  Unidirectional_list::Iterator::first()
{  curr_node = container.head;
   return curr_node ? curr_node->get() : 0;
}
//_first____________________________________________________________2003-06-03_/
Item *  Unidirectional_list::Iterator::last()
{  curr_node = container.tail;
   return curr_node ? curr_node->get() : 0;
}
//_last_____________________________________________________________2006-03-22_/
Item *Unidirectional_list::Iterator::next()
{  if (detached_head)                                                            //161212
   {  curr_node = container.head;                                                //161212
      detached_head = false;                                                     //161212
   } else                                                                        //161212
   {  Unidirectional_list::Node *uni_node = (Unidirectional_list::Node *)curr_node; //060322
      curr_node= uni_node ? uni_node->next : 0;                                     //060322
   }
   return curr_node ? curr_node->get() : 0;
}
//_next_____________________________________________________________2003-06-03_/
Item *  Unidirectional_list::Iterator::prev()
{  // unilist cannot go backwards
   return 0;
}
//_prev_____________________________________________________________2003-06-03_/
Item *  Unidirectional_list::Iterator::current()
{  if (!curr_node) curr_node = container.head;                                   //060322
   return curr_node ? curr_node->get() : 0;                                      //060322
}
//_current_____________________________________________________________________/
Item *  Unidirectional_list::Iterator::Ith(nat32 index)
{  nat32 count = 0;
   for (Node *local_curr = container.head; local_curr; local_curr = local_curr->next)
   {  if (count == index)
      {  curr_node = local_curr;                                                 //060322
         return local_curr->get(); // set curr and return
      }
      count ++;
   }
   return 0;
}
//_Ith______________________________________________________________2003-07-07_/
Item *  Unidirectional_list::Iterator::detach_current()
{  Node *detach_node = (Node *)curr_node;                                        //060322
   Item *detached_item = 0;
   if (detach_node)
   {  detached_item = detach_node->get();
      Node *next_node = detach_node->next;                                       //980206
      Node *prev_node = container.find_prev_of(detach_node);                     //980206
      if (container.head == detach_node)
      {  container.head = detach_node->next;
         detached_head = true;                                                   //161212
      }
      if (prev_node) prev_node->next = next_node;
      curr_node = prev_node;                                                     //160618
      if (detach_node == container.tail)
         container.tail = prev_node;
      if (curr_node == 0)                                                        //060322
         curr_node = container.head;
      detach_node->next = 0;
      detach_node->set(0);  // To prevent deletion of the node data
      delete detach_node;
      container.set_modified(true);                                              //060321
   }
   return detached_item;
}
//_detach_current___________________________________________________2003-07-07_/
bool Unidirectional_list::Iterator::has_successor()                 affirmation_
{  return curr_node && curr_node->next;
}
//_Iterator::has_successor______________________________2018-05-15__2015-05-04_/
bool Unidirectional_list::Iterator::has_predecessor()               affirmation_
{  return false; // Unidirectional list iterator can not go backward
}
//_Iterator::has_predecessor________________________________________2018-05-15_/
}//_namespace CORN_____________________________________________________________/

