#ifndef _EVENT_COUNTER_H_
#define _EVENT_COUNTER_H_

#include <cstdlib>
#include <sstream>
#include <string>
#include <iostream>
#include "avl.h"

namespace cop5536 {
    class EventCounter: private AVL {
    public:
        using key_type = BST::key_type;
        using value_type = BST::value_type;
        using kv_pair = BST::kv_pair;
        using kv_list = BST::kv_list;
        typedef std::vector<value_type> value_list;
    private:
        using super = AVL;
        using typename super::Node;
        bool do_find_next(size_t subtree_root_index, const key_type& search_k, key_type& found_k, value_type& found_v, size_t& nodes_visited) {
            nodes_visited = nodes_visited + 1;
            //do in-order traversal to find the first key which is greater than search_k, while skipping trees that can't possibly be a match
            if (subtree_root_index == 0)
                return false;
            Node const& subtree_root = nodes[subtree_root_index];
            //only search the left subtree if it might contain a key which is greater than search_k
            if (subtree_root.key > search_k) {
                //get smallest left subtree child
                bool found = do_find_next(subtree_root.left_index, search_k, found_k, found_v, nodes_visited);
                if (found)
                    return true;
                //everything in left subtree was less than or equal to search_k, so current node is the first
                //node greater than search_k
                found_k = subtree_root.key;
                found_v = subtree_root.value;
                return true;
            }
            //current node was smaller than or equal to the search key, so check the right subtree
            return do_find_next(subtree_root.right_index, search_k, found_k, found_v, nodes_visited);
        }
        bool do_find_previous(size_t subtree_root_index, const key_type& search_k, key_type& found_k, value_type& found_v, size_t& nodes_visited) {
            nodes_visited = nodes_visited + 1;
            //do in-order traversal backwards to find the first key which is less than search_k, while skipping trees that can't possibly be a match
            if (subtree_root_index == 0)
                return false;
            Node const& subtree_root = nodes[subtree_root_index];
            //only search the right subtree if it might contain a key which is less than search_k
            if (subtree_root.key < search_k) {
                //get largest right subtree child
                bool found = do_find_previous(subtree_root.right_index, search_k, found_k, found_v, nodes_visited);
                if (found)
                    return found;
                //everything in right subtree was greater than or equal to search_k, so current node is the first
                //node less than search_k
                found_k = subtree_root.key;
                found_v = subtree_root.value;
                return true;
            }
            //current node was greater than or equal to the search key, so check the left subtree
            return do_find_previous(subtree_root.left_index, search_k, found_k, found_v, nodes_visited);
        }
        void do_in_range(size_t subtree_root_index, const key_type& k_l, const key_type& k_r, value_list& values, size_t& nodes_visited) {
            nodes_visited = nodes_visited + 1;
            //todo: this should be done in O(lgN + s) time
            if (subtree_root_index == 0)
                return;
            Node const& subtree_root = nodes[subtree_root_index];
            if (subtree_root.key >= k_l) {
                do_in_range(subtree_root.left_index, k_l, k_r, values, nodes_visited);
                if (subtree_root.key <= k_r)
                    values.push_back(subtree_root.value);
            }
            if (subtree_root.key <= k_r)
                do_in_range(subtree_root.right_index, k_l, k_r, values, nodes_visited);
        }
    public:
        EventCounter(size_t init_capacity): super(init_capacity) {}
        EventCounter(kv_list init_kvs): super(init_kvs) {}

        /*
        Increase the count of the event ID by m. If ID is not present, insert it.
        Return the count of ID after the addition.
        */
        uint64_t increase(key_type id, uint64_t m) {
            value_type curr_v(0);
            search(id, curr_v);
            value_type new_v(curr_v + m);
            insert(id, new_v);
            return new_v;
        }

        /*
        Decrease the count of ID by m. If ID’s count becomes less than or equal to 0,
        remove ID from the counter.
        Return the count of ID after the deletion, or 0 if ID is removed or not present.
        */
        uint64_t reduce(key_type id, uint64_t m) {
            value_type curr_v(0);
            search(id, curr_v);
            value_type new_v;
            if (m >= curr_v) {
                remove(id, curr_v);
                new_v = 0;
            } else {
                new_v = curr_v - m;
                insert(id, new_v);
            }
            return new_v;
        }

        /*
        Return ID and count of the event with lowest ID that is greater than ID. Return “0 0” if there is no next ID.
        */
        kv_pair next(key_type id) {
            key_type found_k(0);
            value_type found_v(0);
            size_t nodes_visited = 0;
            do_find_next(root_index, id, found_k, found_v, nodes_visited);
            kv_pair match(found_k, found_v);
            return match;
        }

        /*
        Print ID and count of the event with greatest ID that is less than ID. Print “0 0” if there is no previous ID.
        */
        kv_pair previous(key_type id) {
            key_type found_k(0);
            value_type found_v(0);
            size_t nodes_visited = 0;
            do_find_previous(root_index, id, found_k, found_v, nodes_visited);
            kv_pair match(found_k, found_v);
            return match;
        }

        /*
        Return the count of ID. If not present return 0.
        */
        uint64_t count(key_type id) {
            value_type curr_v(0);
            search(id, curr_v);
            return curr_v;
        }

        /*
        Return the total count for IDs between ID1 and ID2 inclusively. Note ID1 ≤ ID2 .
        */
        void in_range(key_type id1, key_type id2, value_list& values) {
            size_t nodes_visited = 0;
            do_in_range(root_index, id1, id2, values, nodes_visited);
        }
    };
}

#endif
