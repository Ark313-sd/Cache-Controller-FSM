#pragma once
#include <iostream>

class Instruction
{
public:
    std::string x_addr;
    std::string y_addr;
    enum instructionType
    {
        addWord, addByte, subWord, subByte
    };
    instructionType iType;
    Instruction(instructionType itype, std::string x_addr, std::string y_addr);
};