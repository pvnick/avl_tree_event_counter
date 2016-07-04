#ifndef _EVENT_COUNTER_H_
#define _EVENT_COUNTER_H_

#include <cstdlib>
#include <sstream>
#include <string>
#include "avl.h"

namespace cop5536 {
    class EventCounter: private AVL {
    public:
        using key_type = BST::key_type;
        using value_type = BST::value_type;
        using kv_pair = BST::kv_pair;
        using kv_list = BST::kv_list;
    private:
        using super = AVL;
        using typename super::Node;
    public:
        EventCounter(size_t init_capacity): super(init_capacity) {}
        EventCounter(kv_list init_kvs): super(init_kvs) {}

        /*
        Increase the count of the event ID by m. If ID is not present, insert it.
        Return the count of ID after the addition.
        */
        uint64_t increase(key_type id, uint64_t m) {
        }

        /*
        Decrease the count of ID by m. If ID’s count becomes less than or equal to 0,
        remove ID from the counter.
        Return the count of ID after the deletion, or 0 if ID is removed or not present.
        */
        uint64_t reduce(key_type id, uint64_t m) {
        }

        /*
        Return ID and count of the event with lowest ID that is greater than ID. Return “0 0” if there is no next ID.
        */
        kv_pair next(key_type id) {
        }

        /*
        Return the count of ID. If not present return 0.
        */
        uint64_t count(key_type id) {
        }

        /*
        Return the total count for IDs between ID1 and ID2 inclusively. Note ID1 ≤ ID2 .
        */
        uint64_t in_range(key_type id) {
        }
    };
}

#endif
