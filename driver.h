#ifndef _DRIVER_H_
#define _DRIVER_H_

#include "event_counter.h"

#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <string.h>

namespace cop5536 {
    class Driver {
    private:
        EventCounter ec;
        typedef std::vector<std::string> str_list;
        void split(const std::string& str, const std::string& delim, std::vector<std::string>& parts) {
            //get an input string and split it by a given token, pushing the components into a specified output list
            char *str_cpy = (char*)malloc(str.size() + 1),
                *part;
            strcpy(str_cpy, str.c_str());
            part = strtok(str_cpy, delim.c_str());
            while (part != nullptr) {
                std::string str_part(part);
                parts.push_back(str_part);
                part = strtok(nullptr, delim.c_str());
            }
            free(str_cpy);
        }

        std::string str_to_lcase(std::string str) {
            //make a copy of an input string, with all letters converted to lowercase
            std::transform(str.begin(), str.end(), str.begin(), [](char c) {
                if (c >= 'A' && c <= 'Z')
                    return (char)(c - ('Z' - 'z'));
                return (char)c;
            });
            return str;
        }

        void read_kvs(std::ifstream& if_handle, EventCounter::kv_list& kvs_out) {
            //given an input file handle, read all valid key-value pairs
            std::string line;
            str_list parts;
            while (std::getline (if_handle, line))
            {
                parts.clear();
                split(line, " ", parts);
                if (parts.size() == 2) {
                    uint64_t k(std::stoull(parts[0]));
                    uint64_t v(std::stoull(parts[1]));
                    EventCounter::kv_pair kv(k, v);
                    kvs_out.push_back(kv);
                }
            }
        }

        bool read_inp_f(std::string const& if_name, EventCounter::kv_list& kvs) {
            //given an input file name, read all valid key-value pairs
            std::ifstream if_handle(if_name);
            if ( ! if_handle.is_open()) {
                std::cout << "Could not open input file " << if_name << std::endl;
                return false;
            }
            read_kvs(if_handle, kvs);
            if_handle.close();
            return true;
        }

        /*
        Increase the count of the event ID by m. If ID is not present, insert it.
        Print the count of ID after the addition.
        */
        bool increase(str_list const& parts) {
            if (parts.size() != 3)
                return false;
            uint64_t id = std::stoull(parts[1]);
            uint64_t m = std::stoull(parts[2]);
            std::cout << ec.increase(id, m) << std::endl;
            return true;
        }

        /*
        Decrease the count of ID by m. If ID’s count becomes less than or equal to 0,
        remove ID from the counter.
        Print the count of ID after the deletion, or 0 if ID is removed or not present.
        */
        bool reduce(str_list const& parts) {
            if (parts.size() != 3)
                return false;
            uint64_t id = std::stoull(parts[1]);
            uint64_t m = std::stoull(parts[2]);
            std::cout << ec.reduce(id, m) << std::endl;
            return true;
        }

        /*
        Print the total count for IDs between ID1 and ID2 inclusively. Note ID1 ≤ ID2 .
        */
        bool inrange(str_list const& parts) {
            if (parts.size() != 3)
                return false;
            uint64_t id1 = std::stoull(parts[1]);
            uint64_t id2 = std::stoull(parts[2]);
            EventCounter::value_list values;
            ec.in_range(id1, id2, values);
            bool prepend_space = false;
            for (EventCounter::value_type const& val: values) {
                if (prepend_space)
                    std::cout << ' ';
                else
                    prepend_space = true;
                std::cout << val;
            }
            std::cout << std::endl;
            return true;
        }

        /*
        Print ID and count of the event with lowest ID that is greater than ID. Print “0 0” if there is no next ID.
        */
        bool next(str_list const& parts) {
            if (parts.size() != 2)
                return false;
            uint64_t id = std::stoull(parts[1]);
            EventCounter::kv_pair match = ec.next(id);
            std::cout << match.first << ' ' << match.second << std::endl;
            return true;
        }

        /*
        Print ID and count of the event with greatest ID that is less than ID. Print “0 0” if there is no previous ID.
        */
        bool previous(str_list const& parts) {
            if (parts.size() != 2)
                return false;
            uint64_t id = std::stoull(parts[1]);
            EventCounter::kv_pair match = ec.previous(id);
            std::cout << match.first << ' ' << match.second << std::endl;
            return true;
        }

        /*
        Print the count of ID. If not present print 0.
        */
        bool count(str_list const& parts) {
            if (parts.size() != 2)
                return false;
            uint64_t id = std::stoull(parts[1]);
            std::cout << ec.count(id) << std::endl;
            return true;
        }
    public:
        Driver(): ec(1) { }
        bool load_file(std::string inp_f) {
            //set the current copy of the vent counter to one instantiated with the given input file name
            EventCounter::kv_list kvs;
            if ( ! read_inp_f(inp_f, kvs))
                return false;
            EventCounter new_ec(kvs);
            ec = new_ec;
            return true;
        }
        void run_cmd(std::string const& line) {
            //get input lines, parse them, then run the associated commands with given parameters
            str_list parts;
            split(line, " ", parts);
            if (parts.size() == 0) return;
            std::string cmd = str_to_lcase(parts[0]);
            try {
                if (cmd == "increase") {
                    increase(parts);
                } else if (cmd == "reduce") {
                    reduce(parts);
                } else if (cmd == "inrange") {
                    inrange(parts);
                } else if (cmd == "next") {
                    next(parts);
                } else if (cmd == "previous") {
                    previous(parts);
                } else if (cmd == "count") {
                    count(parts);
                } else if (cmd == "quit") {
                    exit(0);
                }
            } catch (std::exception& e) {
                std::cout << "Exception: " << e.what() << std::endl;
            }
        }
    };
}

#endif
