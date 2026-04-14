#include <iostream>
#include <vector>
#include "config.hpp"
#include "Controller.hpp"
#include "Cache.hpp"
using namespace std;




int main()
{
    // need only be created once
    // createRAM();


    // cf.line[0]->setMetadata("00000000", true, false);
    // vector<string> data = {"10110011100011110000111100110011", "0000", "1111", "1010"};
    // cf.line[0]->insertData(data);
    // cf.printCache();
    
    Cache* cache = new Cache(3, 2);
    Controller c(cache);
    c.addInstruction(Instruction(Instruction::addWord, "0", "10000"));
    c.addInstruction(Instruction(Instruction::addByte, "1", "10101"));
    c.operate();




    return 0;
}