#pragma once
#include <iostream>

enum instructionType
{
    add, sub
};
class Instruction
{
public:
    std::string x_addr;
    std::string y_addr;
    instructionType iType;
    Instruction(instructionType itype, std::string x_addr, std::string y_addr);
};