#ifndef _BST_H_
#define _BST_H_

#include <cstdlib>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <set>

namespace cop5536 {
    class BST {
    public:
        typedef uint64_t key_type;
        typedef uint64_t value_type;
        typedef std::pair<key_type, value_type> kv_pair;
        typedef std::vector<kv_pair> kv_list;
    protected:
        struct Node;
        struct Node {
            key_type key;
            value_type value;
            size_t num_children;
            size_t left_index;
            size_t right_index;
            size_t height; //height-tracking so we can look that value up in O(1) time
            bool is_occupied;
            Node(): num_children(0), left_index(0), right_index(0), height(0), is_occupied(0) {}
            size_t validate_children_count_recursive(Node* nodes) {
                //this function is for debugging purposes, does recursive traversal to find the correct number of children
                size_t child_count = 0;
                if (left_index)
                    child_count += 1 + nodes[left_index].validate_children_count_recursive(nodes);
                if (right_index)
                    child_count += 1 + nodes[right_index].validate_children_count_recursive(nodes);
                if (child_count != num_children) {
                    std::ostringstream msg;
                    msg << "Manually counted children, " << child_count << ", different than child count, " << num_children;
                    throw std::logic_error(msg.str());
                }
                return child_count;
            }
            size_t get_height_recursive(Node* nodes) {
                //this function is for debugging purposes, does recursive traversal to find the correct height
                size_t left_height = 0, right_height = 0;
                size_t calculated_height = 0;
                if (left_index)
                    left_height = nodes[left_index].get_height_recursive(nodes);
                if (right_index)
                    right_height = nodes[right_index].get_height_recursive(nodes);
                calculated_height = 1 + std::max(left_height, right_height);
                return calculated_height;
            }
            void update_height(Node* nodes) {
                //note: this method depends on the left and right subtree heights being correct
                size_t left_height = 0, right_height = 0;
                if (left_index)
                    left_height = nodes[left_index].height;
                if (right_index)
                    right_height = nodes[right_index].height;
                height = 1 + std::max(left_height, right_height);
                if (_DEBUG_) {
                    size_t calculated_height = get_height_recursive(nodes);
                    if (calculated_height != height) {
                        std::ostringstream msg;
                        msg << "Manually calculated height, " << calculated_height << ", different than tracked height, " << height;
                        throw std::logic_error(msg.str());
                    }
                }
            }
            void disable_and_adopt_free_tree(size_t free_index) {
                is_occupied = false;
                height = 0;
                num_children = 0;
                right_index = 0;
                left_index = free_index;
            }
            void reset_and_enable(key_type const& new_key, value_type const& new_value) {
                is_occupied = true;
                height = 1; //self
                left_index = right_index = 0;
                num_children = 0;
                key = new_key;
                value = new_value;
            }
            int balance_factor(const Node* nodes) const {
                size_t left_height = 0, right_height = 0;
                if (left_index)
                    left_height = nodes[left_index].height;
                if (right_index)
                    right_height = nodes[right_index].height;
                return static_cast<long int>(left_height) - static_cast<long int>(right_height);
            }
        };
        Node* nodes; //***note: array is 1-based so leaf nodes have child indices set to zero
        size_t free_index;
        size_t root_index;
        size_t curr_capacity;
        virtual size_t remove_smallest_key_node_index(size_t& subtree_root_index) {
            //returns the index of the node with the smallest key, while
            //setting its parent's left child index to the smallest key node's
            //right child index. recursion downward through this function updates
            //the heights of the nodes it traverses
            Node& subtree_root = nodes[subtree_root_index];
            size_t smallest_key_node_index = 0;
            if (subtree_root_index == 0) {
                throw std::logic_error("Expected to find a valid node, but didn't");
            } else {
                if (subtree_root.left_index) {
                    smallest_key_node_index = remove_smallest_key_node_index(subtree_root.left_index);
                    subtree_root.num_children--;
                    subtree_root.update_height(nodes);
                } else {
                    smallest_key_node_index = subtree_root_index;
                    subtree_root_index = subtree_root.right_index;
                }
            }
            return smallest_key_node_index;
        }
        virtual size_t remove_largest_key_node_index(size_t& subtree_root_index) {
            //returns the index of the node with the largest key, while
            //setting its parent's right child index to the largest key node's
            //left child index. recursion downward through this function updates
            //the heights of the nodes it traverses
            Node& subtree_root = nodes[subtree_root_index];
            size_t largest_key_node_index = 0;
            if (subtree_root_index == 0) {
                throw std::logic_error("Expected to find a valid node, but didn't");
            } else {
                if (subtree_root.right_index) {
                    largest_key_node_index = remove_largest_key_node_index(subtree_root.right_index);
                    subtree_root.num_children--;
                    subtree_root.update_height(nodes);
                } else {
                    largest_key_node_index = subtree_root_index;
                    subtree_root_index = subtree_root.left_index;
                }
            }
            return largest_key_node_index;
        }
        virtual void remove_node(size_t& subtree_root_index) {
            Node& subtree_root = nodes[subtree_root_index];
            size_t index_to_delete = subtree_root_index;
            if (subtree_root.right_index || subtree_root.left_index) {
                //subtree has at least one child
                if (subtree_root.right_index)
                    //replace the root with the smallest-keyed node in the right subtree
                    subtree_root_index = remove_smallest_key_node_index(subtree_root.right_index);
                else if (subtree_root.left_index)
                    //replace the root with the largest-keyed node in the left subtree
                    subtree_root_index = remove_largest_key_node_index(subtree_root.left_index);
                //have the new root adopt the old root's children
                Node& new_root = nodes[subtree_root_index];
                new_root.left_index = subtree_root.left_index;
                new_root.right_index = subtree_root.right_index;
                //the new root has the same number of children as the old root, minus one
                new_root.num_children = subtree_root.num_children - 1;
                //removing the smallest/largest-keyed node from the old root has the effect of
                //updating the heights of the old root's relevant subtrees (which the new root
                //just adopted), so we can update the new root's height now
                new_root.update_height(nodes);
            } else
                //neither subtree exists, so just delete the node
                subtree_root_index = 0;
            //node has been disowned by all ancestors, and has disowned all descendents, so free it
            add_node_to_free_tree(index_to_delete);
        }
        virtual int do_remove(size_t nodes_visited, //starts at 0 when this function is first called (ie does not include current node visitation)
                              size_t& subtree_root_index,
                              key_type const& key,
                              value_type& value,
                              bool& found_key)
        {
            if (subtree_root_index != 0) {
                Node& subtree_root = nodes[subtree_root_index];
                ++nodes_visited;
                //keep going down to the base of the tree
                if (key < subtree_root.key) {
                    nodes_visited = do_remove(nodes_visited, subtree_root.left_index, key, value, found_key);
                    if (found_key) {
                        //found the desired node and delete it
                        subtree_root.num_children--;
                        //left child changed, so recompute subtree height
                        subtree_root.update_height(nodes);
                    }
                } else if (key > subtree_root.key) {
                    nodes_visited = do_remove(nodes_visited, subtree_root.right_index, key, value, found_key);
                    if (found_key) {
                        //found the desired node and delete it
                        subtree_root.num_children--;
                        //right child changed, so recompute subtree height
                        subtree_root.update_height(nodes);
                    }
                } else if (key == subtree_root.key) {
                    //found key, remove the node
                    found_key = true;
                    value = subtree_root.value;
                    remove_node(subtree_root_index);
                } else {
                    throw std::logic_error("Unexpected compare result");
                }
            }
            return nodes_visited;
        }
        void write_subtree_buffer(size_t subtree_root_index,
                                  std::vector<std::string>& buffer_lines,
                                  size_t root_line_index,
                                  size_t lbound_line_index /*inclusive*/,
                                  size_t ubound_line_index /*exclusive*/) const
        {
            Node subtree_root = nodes[subtree_root_index];
            std::ostringstream oss;
            //print the node
            oss << "[" << subtree_root.key << ": " << subtree_root.value << "] (" << subtree_root.balance_factor(nodes) << ")";
            buffer_lines[root_line_index] += oss.str();
            //print the right descendents
            if (subtree_root.right_index > 0) {
                //at least 1 right child
                size_t top_dashes = 1;
                Node const& right_child = nodes[subtree_root.right_index];
                if (right_child.left_index > 0) {
                    //right child has at least 1 left child
                    Node const& right_left_child = nodes[right_child.left_index];
                    top_dashes += 2 * (1 + right_left_child.num_children);
                }
                size_t top_line_index = root_line_index - 1;
                while (top_line_index >= root_line_index - top_dashes)
                    buffer_lines[top_line_index--] += "|  ";
                size_t right_child_line_index = top_line_index;
                buffer_lines[top_line_index--] += "+--";
                while (top_line_index >= lbound_line_index)
                    buffer_lines[top_line_index--] += "   ";
                write_subtree_buffer(subtree_root.right_index,
                                     buffer_lines,
                                     right_child_line_index,
                                     lbound_line_index,
                                     root_line_index);
            }
            //print the left descendents
            if (subtree_root.left_index > 0) {
                //at least 1 left child
                size_t bottom_dashes = 1;
                Node const& left_child = nodes[subtree_root.left_index];
                if (left_child.right_index > 0) {
                    //left child has at least 1 right child
                    Node const& left_right_child = nodes[left_child.right_index];
                    bottom_dashes += 2 * (1 + left_right_child.num_children);
                }
                size_t bottom_line_index = root_line_index + 1;
                while (bottom_line_index <= root_line_index + bottom_dashes)
                    buffer_lines[bottom_line_index++] += "|  ";
                size_t left_child_line_index = bottom_line_index;
                buffer_lines[bottom_line_index++] += "+--";
                while (bottom_line_index < ubound_line_index)
                    buffer_lines[bottom_line_index++] += "   ";
                write_subtree_buffer(subtree_root.left_index,
                                     buffer_lines,
                                     left_child_line_index,
                                     root_line_index + 1,
                                     ubound_line_index);
            }
        }
        void add_node_to_free_tree(size_t node_index) {
            nodes[node_index].disable_and_adopt_free_tree(free_index);
            nodes[node_index].num_children = 1 + nodes[nodes[node_index].left_index].num_children;
            free_index = node_index;
        }
        size_t procure_node(key_type const& key, value_type const& value) {
            //updates the free index to the first free node's left child (while transforming that first free
            //node to an enabled node with the specified key/value) and returns the index of what was the last
            //free index
            size_t node_index = free_index;
            free_index = nodes[free_index].left_index;
            Node& n = nodes[node_index];
            n.reset_and_enable(key, value);
            return node_index;
        }
        virtual int insert_at_leaf(size_t nodes_visited, //starts at 0 when this function is first called (ie does not include current node visitation)
                                  size_t& subtree_root_index,
                                  key_type const& key,
                                  value_type const& value,
                                  bool& found_key)
        {
            if (subtree_root_index == 0) {
                //key not found
                subtree_root_index = procure_node(key, value);
            } else {
                //parent was not a leaf
                //keep going down to the base of the tree
                Node& subtree_root = nodes[subtree_root_index];
                ++nodes_visited;
                if (key < subtree_root.key) {
                    nodes_visited = insert_at_leaf(nodes_visited, subtree_root.left_index, key, value, found_key);
                    if ( ! found_key) {
                        //given key is unique to the tree, so a new node was added
                        subtree_root.num_children++;
                        subtree_root.update_height(nodes);
                    }
                } else if (key > subtree_root.key) {
                    nodes_visited = insert_at_leaf(nodes_visited, subtree_root.right_index, key, value, found_key);
                    if ( ! found_key) {
                        //given key is unique to the tree, so a new node was added
                        subtree_root.num_children++;
                        subtree_root.update_height(nodes);
                    }
                } else if (key == subtree_root.key) {
                    //found key, replace the value
                    subtree_root.value = value;
                    found_key = true;
                } else {
                    throw std::logic_error("Unexpected compare result");
                }
            }
            return nodes_visited;
        }
        int do_search(size_t nodes_visited, //starts at 0 when this function is first called (ie does not include current node visitation)
                      size_t subtree_root_index,
                      key_type const& key,
                      value_type& value,
                      bool& found_key) const
        {
            if (subtree_root_index != 0) {
                Node const& subtree_root = nodes[subtree_root_index];
                ++nodes_visited;
                if (key < subtree_root.key) {
                    nodes_visited = do_search(nodes_visited, subtree_root.left_index, key, value, found_key);
                } else if (key > subtree_root.key) {
                    nodes_visited = do_search(nodes_visited, subtree_root.right_index, key, value, found_key);
                } else if (key == subtree_root.key) {
                    //found key, replace the value
                    value = subtree_root.value;
                    found_key = true;
                } else {
                    throw std::logic_error("Unexpected compare result");
                }
            }
            return nodes_visited;
        }
        void remove_ith_node_inorder(size_t& subtree_root_index,
                                     size_t& ith_node_to_delete,
                                     key_type& key)
        {
            Node& subtree_root = nodes[subtree_root_index];
            if (subtree_root.left_index)
                remove_ith_node_inorder(subtree_root.left_index, ith_node_to_delete, key);
            if (ith_node_to_delete == 0)
                //deleted node in child subtree; nothing more to do
                return;
            if (--ith_node_to_delete == 0) {
                //delete the current node
                value_type dummy_val;
                remove(subtree_root.key, dummy_val);
                key = subtree_root.key;
                return;
            }
            if (subtree_root.right_index)
                remove_ith_node_inorder(subtree_root.right_index, ith_node_to_delete, key);
        }

        void increase_capacity() {
            size_t old_capacity = capacity(),
                new_capacity = old_capacity * 2;
            Node* new_nodes = new Node[new_capacity + 1];
            //copy the old tree to the new array
            for (size_t i = 1; i <= old_capacity; ++i)
                new_nodes[i] = nodes[i];
            //delete the old tree and set the pointer to the new array
            delete[] nodes;
            nodes = new_nodes;
            curr_capacity = new_capacity;
            //fill the free tree with the new, unused nodes
            free_index = old_capacity + 1;
            for (size_t i = free_index; i != new_capacity; ++i)
                nodes[i].disable_and_adopt_free_tree(i + 1);
        }
        /*
        start_idx is inclusive
        end_idx is exclusive
        return the new index of subtree root in the nodes array
        */
        virtual size_t init_from_kv_list(const kv_list& init_kvs, const size_t start_idx, const size_t end_idx) {
            if (start_idx == end_idx)
                return 0;
            size_t root_src_idx = (end_idx + start_idx) / 2;
            kv_pair kv = init_kvs[root_src_idx];
            size_t root_dst_idx = procure_node(kv.first, kv.second);
            Node& n = nodes[root_dst_idx];
            n.left_index = init_from_kv_list(init_kvs, start_idx, root_src_idx);
            if (n.left_index)
                n.num_children = 1 + nodes[n.left_index].num_children;
            n.right_index = init_from_kv_list(init_kvs, root_src_idx + 1, end_idx);
            if (n.right_index)
                n.num_children += 1 + nodes[n.right_index].num_children;
            if (_DEBUG_)
                n.validate_children_count_recursive(nodes);
            n.height = 1 + std::max(nodes[n.left_index].height, nodes[n.right_index].height);
            return root_dst_idx;
        }
    public:
        /*
            The constructor will allocate an array of capacity (binary
            tree) nodes. Then make a chain from all the nodes (e.g.,
            make node 2 the left child of node 1, make node 3 the left
            child of node 2, &c. this is the initial free list.
        */
        BST(size_t init_capacity):
            curr_capacity(init_capacity)
        {
            if (init_capacity == 0) {
                throw std::domain_error("init_capacity must be at least 1");
            }
            nodes = new Node[init_capacity + 1];
            clear();
        }
        BST(const kv_list& init_kvs): BST(init_kvs.size() * 2) {
            root_index = init_from_kv_list(init_kvs, 0, init_kvs.size());
        }
        BST::~BST() {
            if (nodes) {
                std::cout << "deleting nodes" << std::endl;
                delete[] nodes;
            }
        }
        /*
            Adds the specified key/value-pair to the tree and returns the number of
            nodes visited, V. Increases capacity if necessary. If an item already
            exists in the tree with the same key, replace its value.
        */
        virtual int insert(key_type const& key, value_type const& value) {
            if (size() == capacity()) {
                //no more space - need to increase the capacity
                increase_capacity();
            }
            bool found_key = false;
            key_type k(key);
            value_type v(value);
            int nodes_visited = insert_at_leaf(0, root_index, k, v, found_key);
            if (_DEBUG_)
                this->nodes[this->root_index].validate_children_count_recursive(this->nodes);
            return nodes_visited;
        }
        /*
            if there is an item matching key, removes the key/value-pair from the tree, stores
            it's value in value, and returns the number of probes required, V; otherwise returns -1 * V.
        */
        virtual int remove(key_type const& key, value_type& value) {
            if (is_empty())
                return 0;
            bool found_key = false;
            key_type k(key);
            value_type v(value);
            int nodes_visited = do_remove(0, root_index, k, v, found_key);
            if (_DEBUG_)
                this->nodes[this->root_index].validate_children_count_recursive(this->nodes);
            if (found_key)
                value = v;
            return found_key ? nodes_visited : -1 * nodes_visited;
        }
        /*
            if there is an item matching key, stores it's value in value, and returns the number
            of nodes visited, V; otherwise returns -1 * V. Regardless, the item remains in the tree.
        */
        virtual int search(key_type const& key, value_type& value) {
            if (is_empty())
                return 0;
            bool found_key = false;
            key_type k(key);
            value_type v(value);
            int nodes_visited = do_search(0, root_index, k, v, found_key);
            if (found_key)
                value = v;
            return found_key ? nodes_visited : -1 * nodes_visited;
        }
        /*
            removes all items from the map
        */
        virtual void clear() {
            //Since I use size_t to hold the node indices, I make the node array
            //1-based, with child index of 0 indicating that the current node is a leaf
            for (size_t i = 1; i != capacity(); ++i)
                nodes[i].disable_and_adopt_free_tree(i + 1);
            free_index = 1;
            root_index = 0;
        }
        /*
            returns true IFF the map contains no elements.
        */
        virtual bool is_empty() const {
            return size() == 0;
        }
        /*
            returns the number of slots in the backing array.
        */
        virtual size_t capacity() const {
            return curr_capacity;
        }
        /*
            returns the number of items actually stored in the tree.
        */
        virtual size_t size() const {
            if (root_index == 0) return 0;
            Node const& root = nodes[root_index];
            return 1 + root.num_children;
        }
        /*
            prints the tree in the following format:
            +--[tiger: tiger val]
            |  |
            |  |  +--[panther: panther val]
            |  |  |
            |  +--[ocelot: ocelot val]
            |     |
            |     +--[lion: lion val]
            |
            [leopard: leopard val]
            |
            |     +--[house cat: house cat val]
            |     |
            |  +--[cougar: cougar val]
            |  |
            +--[cheetah: cheetah val]
               |
               +--[bobcat: bobcat val]
        */
        virtual std::ostream& print(std::ostream& out) const {
            if (is_empty())
                return out;
            size_t num_lines = size() * 2 - 1;
            std::vector<std::string> buffer_lines;
            for(size_t i = 0; i <= num_lines; ++i)
                buffer_lines.push_back("");
            Node const& root = nodes[root_index];
            size_t root_line_index = 1;
            if (root.right_index) {
                root_line_index += 2 * (1 + nodes[root.right_index].num_children);
            }
            write_subtree_buffer(root_index, buffer_lines, root_line_index, 1, num_lines + 1);
            for (size_t i = 1; i <= num_lines; ++i)
                out << buffer_lines[i] << std::endl;
            return out;
        }

    };
}

#endif
