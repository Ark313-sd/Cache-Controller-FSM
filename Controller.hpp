#pragma once
#include "Cache.hpp"
#include "config.hpp"
#include "Instruction.hpp"
#include <queue>

class Controller
{
    Cache *cache;
    
    enum State
    {
        idle, compareTag, writeBack, allocate 
    };
    State state; 
    std::queue<Instruction> pendingInstructions;
    bool fetchBlock(const std::string addr);
    std::string getTag(const std::string& addr) const;
    bool hit(const std::string& addr) const;
    int getBlockIdx(const std::string& addr) const;
    
public:
    Controller();
    void operate();
    void exeInstruction(Instruction i);
    void addInstruction(Instruction i);
};
unsigned int binToInt(const std::string& bits);