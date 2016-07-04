#ifndef _AVL_H_
#define _AVL_H_

#include <cstdlib>
#include <sstream>
#include <string>
#include "bst.h"

namespace cop5536 {
    class AVL: public BST {
    /*
        The trick to AVL is to perform standard BST operations, but wrap recursive methods that might unbalance
        the tree with methods that rebalance the tree after performing those operations. Thus the balance factor
        of any given node stays within [-1, 1]. To that end we simply inherit from a BST base class that tracks
        changes in subtree height and overwrite the needed virtual methods.
    */
    protected:
        using super = BST;
        using typename super::Node;
        int insert_at_leaf(size_t nodes_visited,
                           size_t& subtree_root_index,
                           key_type const& key,
                           value_type const& value,
                           bool& found_key)
        {
            nodes_visited = super::insert_at_leaf(nodes_visited, subtree_root_index, key, value, found_key);
            balance(subtree_root_index);
            return nodes_visited;
        }
        size_t remove_smallest_key_node_index(size_t& subtree_root_index) {
            size_t smallest_key_node_index = super::remove_smallest_key_node_index(subtree_root_index);
            balance(subtree_root_index);
            return smallest_key_node_index;
        }
        size_t remove_largest_key_node_index(size_t& subtree_root_index) {
            size_t largest_key_node_index = super::remove_largest_key_node_index(subtree_root_index);
            balance(subtree_root_index);
            return largest_key_node_index;
        }
        int do_remove(size_t nodes_visited, //starts at 0 when this function is first called (ie does not include current node visitation)
                      size_t& subtree_root_index,
                      key_type const& key,
                      value_type& value,
                      bool& found_key)
        {
            nodes_visited = super::do_remove(nodes_visited, subtree_root_index, key, value, found_key);
            balance(subtree_root_index);
            return nodes_visited;
        }
        void rotate_left(size_t& subtree_root_index) {
            Node& subtree_root = nodes[subtree_root_index];
            size_t right_child_index = subtree_root.right_index;
            Node& right_child = nodes[right_child_index];

            //original root adopts the right child's left subtree
            subtree_root.right_index = right_child.left_index;
            //original root adopted a subtree (whose height did not change), so update its height
            subtree_root.update_height(nodes);

            //right child adopts original root and its children
            right_child.left_index = subtree_root_index;
            //right child (new root) adopted the original root (whose height has been updated), so update its height
            right_child.update_height(nodes);
            //since right child took the subtree root's place, it has the same number of children as the original root
            right_child.num_children = subtree_root.num_children;

            //root has new children, so update that counter (done after changing the right child's children counter
            //because that depends on the original root's counter)
            subtree_root.num_children = 0;
            if (subtree_root.left_index != 0)
                subtree_root.num_children += 1 + nodes[subtree_root.left_index].num_children;
            if (subtree_root.right_index != 0)
                subtree_root.num_children += 1 + nodes[subtree_root.right_index].num_children;

            //set the right child as the new root
            subtree_root_index = right_child_index;
        }
        void rotate_right(size_t& subtree_root_index) {
            Node& subtree_root = nodes[subtree_root_index];
            size_t left_child_index = subtree_root.left_index;
            Node& left_child = nodes[left_child_index];

            //original root adopts the left child's right subtree
            subtree_root.left_index = left_child.right_index;
            //original root adopted a subtree (whose height did not change), so update its height
            subtree_root.update_height(nodes);

            //left child adopts original root and its children
            left_child.right_index = subtree_root_index;
            //left child (new root) adopted the original root (whose height has been updated), so update its height
            left_child.update_height(nodes);
            //since left child took the subtree root's place, it has the same number of children as the original root
            left_child.num_children = subtree_root.num_children;

            //root has new children, so update that counter (done after changing the left child's children counter
            //because that depends on the original root's counter)
            subtree_root.num_children = 0;
            if (subtree_root.left_index != 0)
                subtree_root.num_children += 1 + nodes[subtree_root.left_index].num_children;
            if (subtree_root.right_index != 0)
                subtree_root.num_children += 1 + nodes[subtree_root.right_index].num_children;

            //set the left child as the new root
            subtree_root_index = left_child_index;
        }
        void balance(size_t& subtree_root_index) {
            if (subtree_root_index == 0) return;
            Node& root = this->nodes[subtree_root_index];
            int root_bal_fact = root.balance_factor(this->nodes);
            if (root_bal_fact == -2) {
                //right subtree is too heavy
                size_t& right_index = root.right_index;
                Node& right_child = this->nodes[right_index];
                switch(right_child.balance_factor(this->nodes)) {
                case 1:
                    //right left
                    this->rotate_right(right_index);
                    this->rotate_left(subtree_root_index);
                    break;
                case -1:
                case 0:
                    //right right
                    this->rotate_left(subtree_root_index);
                    break;
                default:
                    std::ostringstream err;
                    err << "Unexpected balance factor with heavy right subtree: "
                        << right_child.balance_factor(this->nodes);
                    throw std::domain_error(err.str());
                }
            } else if (root_bal_fact == 2) {
                //left subtree is too heavy
                size_t& left_index = root.left_index;
                Node& left_child = this->nodes[left_index];
                switch(left_child.balance_factor(this->nodes)) {
                case -1:
                    //left right
                    this->rotate_left(left_index);
                    this->rotate_right(subtree_root_index);
                    break;
                case 1:
                case 0:
                    //left left
                    this->rotate_right(subtree_root_index);
                    break;
                default:
                    std::ostringstream err;
                    err << "Unexpected balance factor with heavy left subtree: "
                        << left_child.balance_factor(this->nodes);
                    throw std::domain_error(err.str());
                }
            } else if (std::abs(root_bal_fact) > 2) {
                std::ostringstream err;
                err << "Unexpected balance factor when checking for heavy subtree: "
                    << root_bal_fact;
                throw std::domain_error(err.str());
            }
        }
        void do_validate_avl_balance(size_t subtree_root_index) const {
            if (_DEBUG_) {
                if (subtree_root_index == 0) return;
                Node const& n = this->nodes[subtree_root_index];
                if (abs(n.balance_factor(this->nodes)) > 1)
                    throw std::domain_error("Unexpected unbalanced tree while checking balance factor of all tree nodes");
                do_validate_avl_balance(n.left_index);
                do_validate_avl_balance(n.right_index);
            }
        }
        void validate_avl_balance() {
            if (_DEBUG_)
                do_validate_avl_balance(this->root_index);
        }
    public:
        AVL(size_t init_capacity): super(init_capacity) {}
        /*
            Initialize an AVL tree using a list of key-values, sorted by key, in O(N) time
        */
        AVL(kv_list init_kvs): super(init_kvs.size()) {
            //
        }
        /*
            Adds the specified key/value-pair to the tree and returns the number of
            nodes visited, V. Increases capacity if necessary. If an item already
            exists in the tree with the same key, replace its value.
        */
        int insert(key_type const& key, value_type const& value) {
            if (this->size() == this->capacity()) {
                //no more space - need to increase the capacity
                increase_capacity();
            }
            bool found_key = false;
            key_type k(key);
            value_type v(value);
            int nodes_visited = insert_at_leaf(0, this->root_index, k, v, found_key);
            validate_avl_balance();
            if (_DEBUG_)
                this->nodes[this->root_index].validate_children_count_recursive(this->nodes);
            return nodes_visited;
        }
        /*
            if there is an item matching key, removes the key/value-pair from the tree, stores
            it's value in value, and returns the number of probes required, V; otherwise returns -1 * V.
        */
        int remove(key_type const& key, value_type& value) {
            if (this->is_empty())
                return 0;
            bool found_key = false;
            key_type k(key);
            value_type v(value);
            int nodes_visited = do_remove(0, this->root_index, k, v, found_key);
            validate_avl_balance();
            if (_DEBUG_)
                this->nodes[this->root_index].validate_children_count_recursive(this->nodes);
            if (found_key)
                value = v;
            return found_key ? nodes_visited : -1 * nodes_visited;
        }
    };
}

#endif
