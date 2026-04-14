#include "Instruction.hpp"
#include "config.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace  std;

Instruction::Instruction(instructionType iType, std::string x_addr, std::string y_addr)
        : iType(iType), x_addr(x_addr), y_addr(y_addr)
{
    stringstream ssx;
    ssx << setw(ADDRESS_SIZE) << setfill('0') << x_addr;
    this->x_addr = ssx.str();

    stringstream ssy;
    ssy << setw(ADDRESS_SIZE) << setfill('0') << y_addr;
    this->y_addr = ssy.str();
}
