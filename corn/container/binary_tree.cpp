#ifndef CORN_BINARY_TREE
#   include "corn/container/binary_tree.h"
#endif
#ifndef CORN_UNI_LIST
#   include "corn/container/unilist.h"
#endif
#include <assert.h>
namespace CORN {
//______________________________________________________________________________
Binary_tree::Node::Node(Item *elmt_data_)
: Container::Node(elmt_data_)
, left(0)
, right(0)
{}
//______________________________________________________________________________
Binary_tree::Node::~Node()
{  /*
   At this point left and right should usually both be guarenteed to be 0.
   Although in the case of delete_all() we will need to make sure to delete
   */
   if (left) delete left;
   if (right) delete right;
   left = 0;
   right = 0;
}
//______________________________________________________________________________
nat32  Binary_tree::Node::get_weight()                                     const
{
// I think I always have to fully recursively count because
// I don't know where detached nodes are taken from
// they can be from any place in the tree and I would need to trace
// to the tree root to fix up the counts
   nat32   node_count = 1
      + (left ? left->get_weight() : 0)
      + (right ? right->get_weight() : 0);
   return node_count;
}
#define INDEX_UNKNOWN 0xFFFFFFFF
//______________________________________________________________________________
Binary_tree::Node *Binary_tree::Node::get_at(nat32 search_index,nat32 &curr_index/*,nat32 &this_nodes_index*/)
{  //Binary_tree::Node *found_node = 0;
   if (left)
   {  Binary_tree::Node *found_node = left->get_at(search_index,curr_index/*,lefts_index*/);
      if (found_node) return found_node;
   } else // nothing to the left so this is a left most node
   {  curr_index = (curr_index == INDEX_UNKNOWN) ?  0 : curr_index + 1;
      if (curr_index == search_index) // this is the node we are looking for
         return this;
   }
   // else this is not the node look right
   if (right)
   {
      Binary_tree::Node *found_node = right->get_at(search_index,curr_index);
      if (found_node) return found_node;
   }
   return 0;
}
//______________________________________________________________________________
Binary_tree::Node *Binary_tree::Node::insert(Binary_tree::Node *node,bool keep_balance)
{  int comparison = compare(*(node->get()));
   if (comparison < 0)
   {  // this is less than the node's value
      if (right) right->insert(node,keep_balance);
      else       right = node;
   } else if (comparison > 0)
   {  // this is greater than the node's value
      if (left) left->insert(node,keep_balance);
      else      left = node;
   } else // comparison = 0
   {  // This inserted node has the same value as this
      // Try to add it to either the left or right if the is no subtree
      if (!left)  left = node;
      else if (!right) right = node;
      else // just insert it to the right
         right->insert(node,keep_balance);
   }
   if (keep_balance) rebalance();                                                //051009
   return node;
}
//______________________________________________________________________________
bool Binary_tree::Node::rebalance()
{
   bool balance_was_necessary =false;
   if (!left && right && !right->get_left() && right->get_right())     // If this node (X) has no left node and
   {  Binary_tree::Node *y = right;
      left = new Binary_tree::Node(elmt_data);    // A new left node is added to X with X's data.
      elmt_data = y->get();                      // Y's data is moved to X.
      y->set(0);                                 // clear the pointer it has been relinquished to this
      right = y->relinquish_right();                    // Y's right node is moved to X's right node
      delete y;                                  // and Y is deleted.
      balance_was_necessary = true;
   }
   if (!right&& left  && !left->get_right() && left->get_left())     // If this node (X) has no right node and
   {  Binary_tree::Node *y = left;
      right= new Binary_tree::Node(elmt_data);    // A new right node is added to X with X's data.
      elmt_data = y->get();                      // Y's data is moved to X.
      y->set(0);                                 // clear the pointer it has been relinquished to this
      left = y->relinquish_left();                       // Y's left node is moved to X's left node
      delete y;                                   // and Y is deleted.
      balance_was_necessary = true;
   }
   nat32 left_weight = left ? left->get_weight() : 0;
   nat32 right_weight = right ? right->get_weight() : 0;
   if (left_weight > right_weight+1)            // If at this node X the left node weight is one more greater than the right node
   {
      Binary_tree::Node *move_this = new Binary_tree::Node(elmt_data);
      elmt_data = 0;
      if (right)   right->insert(move_this,true);  // X's data is inserted to the right subtree.
      else right = move_this;  // If the right subtree is empty, the data is added as a right leaf.
      Binary_tree::Node *lefts_rightmost = left->detach_right_most_node();
      if (lefts_rightmost)
      {   elmt_data = lefts_rightmost->get();       // the data for the rightmost leaf of the left node is moved to X's
         lefts_rightmost->set(0);                  // clear the pointer because delete Item deletes the element
         delete lefts_rightmost;                   // and left's right most leaf is deleted.
      }else
         if (!left->right)
         {   Binary_tree::Node *old_left  = left ;
            elmt_data = old_left->get();
            old_left->set(0);
              left = left->left;
              old_left->left = 0;
            delete old_left;
         } else
         {   elmt_data = lefts_rightmost->get();    // the data for the left most leaf of the right node is moved to X's
            lefts_rightmost->set(0);               // clear the pointer because delete Item deletes the element
            delete lefts_rightmost;                // and right's left most leaf is deleted.
         }
      balance_was_necessary = true;
   }
    left_weight = left ? left->get_weight() : 0;
    right_weight = right ? right->get_weight() : 0;
   if (right_weight > left_weight+1)               // If at this node X the right node weight is one more greater than the left node
   {  Binary_tree::Node *move_this = new Binary_tree::Node(elmt_data);
      elmt_data = 0;
      if (left ) left->insert(move_this,true);     // X's data is inserted to the left subtree.
      else       left  = move_this;                  // the data is added as a left  leaf.
      Binary_tree::Node *rights_leftmost = right->detach_left_most_node();
      if (rights_leftmost)
      {   elmt_data = rights_leftmost->get();       // the data for the rightmost leaf of the left node is moved to X's
         rights_leftmost->set(0);                  // clear the pointer because delete Item deletes the element
         delete rights_leftmost;                   // and left's right most leaf is deleted.
      } else
         if (!right->left)
         {   Binary_tree::Node *old_right = right;
            elmt_data = old_right->get();
            old_right->set(0);
              right = right->right;
              old_right->right = 0;
            delete old_right;
         } else
         {   elmt_data = rights_leftmost->get();    // the data for the left most leaf of the right node is moved to X's
            rights_leftmost->set(0);               // clear the pointer because delete Item deletes the element
            delete rights_leftmost;                // and right's left most leaf is deleted.
         }
      balance_was_necessary = true;
   }
   // Otherwise the tree is essentially balanced.
   return balance_was_necessary;
}
//_Node::rebalance__________________________________________________2005-10-09_/
Binary_tree::Node *Binary_tree::Node::detach_left_most_node()
{  // this node is not the left most node, rebalance has already check the condition for the top node
   Binary_tree::Node *detached_node = 0;
   if (left)
   {  if (left->get_left()) // At this point there should always be a left
      detached_node = left->detach_left_most_node();
      else
      {  // left is the left most node
         detached_node = left;
         left = left->relinquish_right();
      }
   }
   return detached_node;
}
   // returns 0 if there was no left node.
//______________________________________________________________________________
Binary_tree::Node *Binary_tree::Node::detach_right_most_node()
{  // this node is not the right most node, rebalance has already check the condition for the top node
   Binary_tree::Node *detached_node = 0;
     if (!right) return 0;                                                       //051009
   if (right->get_right()) // At this point there should always be a right
      detached_node = right->detach_right_most_node();
   else
   {  // right is the right most node
      detached_node = right;
      right = right->relinquish_left();
   }
   return detached_node; // returns 0 if there was no left node.
}
//______________________________________________________________________________
Binary_tree::Node *Binary_tree::Node::detach(Binary_tree::Node *node_to_detach,bool keep_balance)
{  Binary_tree::Node *detached_node = 0;
   // caller has already checked that node is not this
   if (left == node_to_detach)
   {  // we've found what needs to be detached
      if (!left->get_right())
      { //if left has no right simply set left to left's left
         left = left->relinquish_left();
      } else
      { // else if left has no left simply set left to left's right
         if (!left->get_left())
         {  left = left->relinquish_right();
         } else
         {  // insert left's right to left's left
            left->get_left()->insert(left->relinquish_right(),keep_balance);
            // set left to left's left
            left = left->relinquish_left();
         }
      }
      detached_node = node_to_detach;
   }
   else if (right == node_to_detach)
   {  // we've found what needs to be detached
      if (!right->get_left())
      { //if right has no left simply set right to right's right
         right = right->relinquish_right();
      } else
      { // else if right has no right simply set right to right's left
         if (!right->get_right())
         {  right = right->relinquish_left();
         } else
         {  // insert right's left to right's right
            right->get_right()->insert(right->relinquish_left(),keep_balance);
            // set right to right's right
            right = right->relinquish_right();
         }
      }
      detached_node = node_to_detach;
   }
   if (!detached_node) // we haven't found the node yet recurse to left
   if (left)
      detached_node = left->detach(node_to_detach,keep_balance);
   if (!detached_node) // it wasn't to the left try to the right
      detached_node = right->detach(node_to_detach,keep_balance);
   if (detached_node)
   {  if (keep_balance)  rebalance();
      detached_node->clear_left();
      detached_node->clear_right();
   }
   // else, the node was not found here
   return detached_node;
}
//______________________________________________________________________________

/*
Weight balance
 binary trees

The weight of a node is weight of the left node plus the right node plus 1.
The weight of an empty child node is 0
The weight of a leaf is 1
*/

nat32 Binary_tree::count()                                                 const
{  // The root node should always have the number nodes in the tree.
   return root ? root->get_weight() : 0;
}
//______________________________________________________________________________
Item * Binary_tree::take(Item *new_element)
{  if (new_element)
   {  Binary_tree::Node *node = new Binary_tree::Node(new_element);
      if (root)   root->insert(node,false);
      else        root = node;
      set_modified(true);
   }
   if (keep_balance) rebalance();
   return new_element;
}
//______________________________________________________________________________
nat32  Binary_tree::delete_all(Item::Filter filter)
{  nat32 count_deleted = count();
   if (root && (filter == Item::filter_all))
      {delete root; root = 0;}
   else
   {  count_deleted = 0;
      Unidirectional_list not_deleted;
      while (root)
      {  Item *del_el = pop_first();
         if (del_el)
         {  if (del_el->matches_filter(filter))
            {  count_deleted += 1;
               delete del_el;
            } else not_deleted.append(del_el);
         }
      }
      transfer_all_from(not_deleted);
   }
   return count_deleted;
}
//_delete_all_______________________________________________________2004-03-18_/
Item *Binary_tree::pop_first()
{  Item *popped_item = 0;
   if (root)
   {  Binary_tree::Node *popped_node
         = root->detach_left_most_node();
      if (!popped_node)
      {  Binary_tree::Node *old_root =root;
         root = root->relinquish_right();                                        //051009
         popped_node = old_root;
      }
      popped_item = popped_node->get();
      popped_node->set(0); // make sure we don't delete the item                 //051009
      delete popped_node;
      set_modified(true);                                                        //060221
   }
   return popped_item;
}
//_pop_first___________________________________________________________________/
Item *Binary_tree::pop_last()
{  Item *popped_item = 0;
   if (root)
   {  Binary_tree::Node *popped_node
         = root->detach_right_most_node();
      if (!popped_node)
      {  Binary_tree::Node *old_root =root;
         root = root->relinquish_left();                                        //051009
         popped_node = old_root;
      }
      popped_item = popped_node->get();
      popped_node->set(0); // make sure we don't delete the item                 //051009
      delete popped_node;
      set_modified(true);                                                        //060221
   }
   return popped_item;
}
//_pop_last_________________________________________________________2017-12-04_/
bool Binary_tree::rebalance()
{  bool balanced_needed = false;
   if (root) balanced_needed = root->rebalance();  // NYI completely
   if (balanced_needed) while (root->rebalance());
   return balanced_needed;
}
//_rebalance________________________________________________________2005-10-09_/
Item * Binary_tree::get_at(nat32 index)                                    const
{  Item *got_item = 0;
   if (root)
   {  nat32 curr_index = INDEX_UNKNOWN; // root's index is not initially known, it will be returned
      Binary_tree::Node *got_node = root->get_at(index,curr_index);
      if (got_node)
         got_item = got_node->get();
   }
   return got_item;
}
//______________________________________________________________________________
Container::Iterator *Binary_tree::iterator()                               const
{  Binary_tree      &this_container = (Binary_tree      &)*this;                 //030907
   Binary_tree::Iterator *itor = new Binary_tree::Iterator(this_container);
   return itor;
}
//_iterator_________________________________________________________2003-06-03_/
Binary_tree::Iterator::Iterator(Binary_tree &i_container)
: Container::Iterator()
, container(i_container)
, curr(container.root)
, parent_stack_index(0)
{}
//_Iterator:constructor_____________________________________________2003-06-03_/
Binary_tree::Iterator::~Iterator()
{}
//______________________________________________________________________________
Item *  Binary_tree::Iterator::furthest(bool forward)
{  parent_stack_index = 0;
   parent_stack[parent_stack_index] = 0;
   curr = container.root;
   if (curr)
   {  Binary_tree::Node *further = 0;
      while ((further = (forward ? curr->get_right() : curr->get_left())) != 0)
      {  parent_stack[parent_stack_index++]  = curr;
         curr = further;
      }
   }
   return curr ? curr->get() : 0;
}
//_Iterator::furthest_______________________________________________2003-06-03_/
Item *  Binary_tree::Iterator::first()
{  return furthest(false);
}
//_Iterator::first__________________________________________________2003-06-03_/
Item *  Binary_tree::Iterator::last()
{  return furthest(true);
}
//_Iterator::last___________________________________________________2003-06-03_/
Item *  Binary_tree::Iterator::adjacent(bool forward)
{     Binary_tree::Node *toward = forward ? curr->get_right() : curr->get_left();
      if (toward)
      {  parent_stack[parent_stack_index++] = toward;
         Binary_tree::Node *skip = forward ? toward->get_left(): toward->get_right();
         if (skip)
         {   while (skip)
            {  parent_stack[parent_stack_index++] = skip;
               curr = skip;
               skip = forward ? skip->get_left() : skip->get_right();
            }
            if (curr==parent_stack[parent_stack_index-1])
               parent_stack[--parent_stack_index] = 0;
         } else
         {  parent_stack[parent_stack_index--] = 0;
            curr = toward;
         }
      } else
      {  parent_stack[parent_stack_index] = 0; // <- This isn't really necessary, but cleans up for debugger.
         parent_stack_index -= 1;
         curr = parent_stack_index>=0 ? parent_stack[parent_stack_index] : 0;
      }
   return curr ? curr->get() : 0;
}
//_Iterator::adjacent_______________________________________________2003-06-03_/
Item *  Binary_tree::Iterator::next()
{  if (!curr) return first();
   return adjacent(true);
}
//_Iterator::next___________________________________________________2003-06-03_/
Item *  Binary_tree::Iterator::prev()
{  if (!curr) return last();
   return adjacent(false);
}
//_Iterator::prev___________________________________________________2003-06-03_/
Item *  Binary_tree::Iterator::current()
{  if (!curr) curr = container.root;
   return curr ? curr->get() : 0;
}
//_Iterator::current___________________________________________________________/
Item *  Binary_tree::Iterator::Ith(nat32 index)
{  Item *item = 0;
   nat32 curr_index = INDEX_UNKNOWN;
   Binary_tree::Node *node = container.root ? container.root->get_at(index,curr_index) : 0;
   if (node) item = node->get();
   return item;
}
//_Iterator::Ith____________________________________________________2003-07-07_/
Item *  Binary_tree::Iterator::detach_current()
{  Binary_tree::Node *detach_node = curr;
   Item *detached_item = 0;
   if (detach_node)
   {  detached_item = detach_node->get();
      if (container.root == detach_node)
      {  // Special case to delete the root node
         Binary_tree::Node *root_left = container.root->relinquish_left();
         Binary_tree::Node *root_right = container.root->relinquish_right();
         if (!root_left)                 // root only has right (or no) subtree.
            container.root = root_right; // Simply set the root to the right subtree
         else if (!root_right)           // root only has left (or no) subtree.
            container.root = root_left;
         else if (root_left && root_right)
         {  container.root = root_left;
            root_left->insert(root_right,true);
         }
      }
      else
      container.root->detach(detach_node,true);
      detach_node->set(0);  // To prevent deletion of the node data
      delete detach_node;
      curr = 0;
   }
   return detached_item;
}
//_Iterator::detach_current_________________________________________2003-07-07_/
Item *Binary_tree::Iterator::find_comparable(const Item &item_to_find,bool from_beginning)
{  Node *next = 0;
   Node *found = 0;
   for (curr = from_beginning ? container.root : curr
       ;curr && !found
       ;curr = next)
   {  next = 0;
      int comparison = curr->compare(item_to_find);
      if (comparison == 0) found = curr;
      else if (comparison > 0) next = curr->get_left();  // Current node is greater than item_to_find
      else if (comparison < 0) next = curr->get_right();
      if (!next) found = curr;
   }
   curr = found;
   return found ? found->get(): 0;
}
//_Iterator::find_comparable________________________________________2005-11-01_/
bool Binary_tree::Iterator::has_predecessor()                       affirmation_
{
   assert(false); // NYI
   return false;
}
//_Iterator::has_predecessor________________________________________2015-05-05_/
bool Binary_tree::Iterator::has_successor()                         affirmation_
{
   assert(false); // NYI
   return false;
}
//_2015-05-05___________________________________________________________________
void Binary_tree::write_structure(std::ostream &strm)                 stream_IO_
{  if (root) root->write_structure(strm,0);
   else  strm << std::endl;
}
//_write_structure_____________________________________________________________/
void Binary_tree::Node::write_structure(std::ostream &strm,nat32 indentation) stream_IO_
{  for (nat32 i = 0; i < indentation; i++) strm << ' ';
   get()->write(strm);
   strm << std::endl;
   if (left)  left->write_structure(strm,indentation+1);
   if (right) right->write_structure(strm,indentation+1);
}
//_Node::write_structure________________________________________________________/
Binary_tree::~Binary_tree()
{  delete root; root = 0;                                                        //170630
}
//_Binary_tree:destructor___________________________________________2005-10-10_/
}//_namespace CORN_____________________________________________________________/

