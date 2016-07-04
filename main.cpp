#include <iostream>

#define _DEBUG_ true

#include "event_counter.h"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include <fstream>
#include <string>


void split(const std::string& str, const char delim, std::vector<std::string>& parts_out) {
    std::stringstream ss(str);
    std::string part;
    while (std::getline(ss, part, delim))
        parts_out.push_back(part);
}

void read_kvs(std::ifstream& if_handle, cop5536::EventCounter::kv_list& kvs_out) {
    std::string line;
    std::vector<std::string> parts;
    while (std::getline (if_handle, line))
    {
        parts.clear();
        split(line, ' ', parts);
        if (parts.size() == 2) {
            uint64_t k(std::stoull(parts[0]));
            uint64_t v(std::stoull(parts[1]));
            cop5536::EventCounter::kv_pair kv(k, v);
            kvs_out.push_back(kv);
        }
    }
}

bool read_if(const char* if_name, cop5536::EventCounter::kv_list& kvs) {
    std::ifstream if_handle(if_name);
    if ( ! if_handle.is_open()) {
        std::cout << "Could not open input file " << if_name << std::endl;
        return false;
    }
    read_kvs(if_handle, kvs);
    if_handle.close();
    return true;
}

void fuzz_avl_tree() {
    std::srand(std::time(0)); // use current time as seed for random generator
    cop5536::AVL tree(10);
    for (int i = 0; i != 1000000; ++i){
        int op = std::floor(static_cast<float>(std::rand()) / RAND_MAX * 1000);
        uint64_t k = std::floor(static_cast<float>(std::rand()) / RAND_MAX * 100);
        uint64_t v = std::floor(static_cast<float>(std::rand()) / RAND_MAX * 100);
        if (op == 0) {
            tree.clear();
        } else if (op >= 1 && op <= 800)
            tree.insert(k, v);
        else
            tree.remove(k, v);
    }
    tree.print(std::cout);
}


int main( int argc, char* argv[] )
{
    if (argc != 2) {
        std::cout << "Expected first argument to be the input file name" << std::endl;
        return 1;
    }
    cop5536::EventCounter::kv_list kvs;
    if ( ! read_if(argv[1], kvs))
        return 1;
    cop5536::EventCounter ec(kvs);
    fuzz_avl_tree();
    return 0;
}
