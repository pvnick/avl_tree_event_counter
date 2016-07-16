#define _DEBUG_ false

#include "driver.h"

int main( int argc, char* argv[] )
{
    if (argc != 2) {
        std::cout << "Expected first argument to be the input file name" << std::endl;
        return 1;
    }
    std::string inp_f(argv[1]);
    cop5536::Driver driver;
    if ( ! driver.load_file(inp_f))
        return 1;
    while(true) {
        //the only point of main.cpp is to instantiate the driver with the input file and then pass input lines to it
        std::string line;
        std::getline(std::cin, line);
        driver.run_cmd(line);
    }
    return 0;
}
