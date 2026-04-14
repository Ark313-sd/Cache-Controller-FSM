#include "Controller.hpp"
#include "Cache.hpp"
#include "config.hpp"
#include "Instruction.hpp"
#include "fstream"
#include <sstream>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <cmath>
using namespace std;




Controller::Controller(Cache* cache) : state(idle)
{
    this->cache = cache;
    cout << "Tag Size:" << cache->line[0]->getTagSize() << endl;
}
void Controller::operate()
{
    while(!pendingInstructions.empty())
    {
        if(state == idle)
        {
            Instruction i = pendingInstructions.front();
            pendingInstructions.pop();
            long long int res = exeInstruction(i);
            cout << "res: " << res << endl;
        }
    
    }
}
unsigned int binToInt(const std::string& bits)
{
    if (bits.empty())
        throw std::invalid_argument("bit string is empty");

    unsigned int value = 0;

    for (char c : bits)
    {
        if (c != '0' && c != '1')
            throw std::invalid_argument("bit string must contain only 0 and 1");

        value = (value << 1) | (c - '0');
    }

    return value;
}
string Controller::getTag(const string& addr) const
{
    string tag = "";
    int tagSize = ADDRESS_SIZE - log2(cache->nLines) - log2(cache->nWords) - 2;
    for(int i = 0; i < tagSize; i++)
    {
        tag += addr[i];
    }
    return tag;
}
int Controller::getBlockIdx(const std::string& addr) const
{
    int m = log2(cache->nWords);
    int tagSize = cache->line[0]->getTagSize();
    string bin_idx = "";
    
    for(int i = tagSize; i < addr.size() - m - 2; i++)
    {
        bin_idx += addr[i];
    }
    return binToInt(bin_idx);

}
bool Controller::hit(const std::string& addr) const
{
    long long int idx = getBlockIdx(addr);

    if(!cache->line[idx]->isValid())
        return false;

    string addr_tag = getTag(addr);
    string cache_tag = cache->line[idx]->getTag();

    return addr_tag == cache_tag;
}
int Controller::getDataFromCacheByAddr(Instruction::instructionType iType, const string& addr) const
{
    int x = -1;
    switch(iType)
    {
        case Instruction::addWord:
        {
            int idx = getBlockIdx(addr);
            int wordIdx = getWordOffset(addr);
            string x_bin = cache->line[idx]->getWord(wordIdx);
            x = binToInt(x_bin);
            cout << "x_bin: " << x_bin << " x: " << x << endl;
            break;
        }
        case Instruction::addByte:
        {
            int idx = getBlockIdx(addr);
            int wordIdx = getWordOffset(addr);
            int byteOffset = getByteOffset(addr);
            string word = cache->line[idx]->getWord(wordIdx);
            string x_bin = word.substr(8 * byteOffset, 8);
            x = binToInt(x_bin);
            cout << "x_bin: " << x_bin << " x: " << x << endl;
            break;
        }
        case Instruction::subWord:
        {
            int idx = getBlockIdx(addr);
            int wordIdx = getWordOffset(addr);
            x = binToInt(cache->line[idx]->getWord(wordIdx));
            break;
        }
        case Instruction::subByte:
        {
            int idx = getBlockIdx(addr);
            int wordIdx = getWordOffset(addr);
            int byteOffset = getByteOffset(addr);
            string word = cache->line[idx]->getWord(wordIdx);
            x = binToInt(word.substr(8 * byteOffset, 8));
            break;
        }
        default : cout << "Unknown type\n";
    }
    return x;
}
long long int Controller::exeInstruction(Instruction ins)
{
    cout << "X addr: " << ins.x_addr << endl;
    cout << "Y addr: " << ins.y_addr << endl;


    if(!hit(ins.x_addr))
    {
        cout << "cache miss tryna fetch X\n";
        fetchBlock(ins.x_addr);
        cout << "done fetching X\n";
    }
    else
    {
        cout << "cache hit on X\n";
    }

    int x = getDataFromCacheByAddr(ins.iType, ins.x_addr);

    if(!hit(ins.y_addr))
    {
        cout << "cache miss tryna fetch Y\n";
        fetchBlock(ins.y_addr);
        cout << "done fetching Y\n";
    }
    else
    {
        cout << "cache hit on Y\n";
    }

    int y = getDataFromCacheByAddr(ins.iType, ins.y_addr);

    cache->printCache();

    
    switch(ins.iType)
    {
        case Instruction::addWord:
        {
            return x + y;
        }
        case Instruction::addByte:
        {
            return x + y;
        }
        case Instruction::subWord:
        {
            return x - y;
        }
        case Instruction::subByte:
        {
            return x - y;
        }
        default : cout << "unknown instruction type\n";
    }
    return 0;
}
bool Controller::fetchBlock(const std::string addr)
{
    long long int memBlockIdx = binToInt(addr) / 4;

    ifstream file("ram.txt", ios::in);

    if(file.is_open() && !file.fail())
    {
        cout << "ram opened\n";
        stringstream ss;
        for(int i = 0; i < memBlockIdx * 4; i++)
        {
            string buffer;
            getline(file, buffer);
        }
        cout << "ram traversed\n";

        vector<string> words(cache->nWords);
        for(int i = 0; i < cache->nWords; i++)
        {
            string buffer;
            for(int j = 0; j < 4; j++)
            {
                getline(file, buffer, ' ');
                // cout << "buffer discarded\n";
                getline(file, buffer);
                // cout << "byte taken\n";
                words[i] += buffer;
                // cout << "byte added\n";
            }
        }

        int idx = getBlockIdx(addr);
        (*this->cache)[idx]->insertData(words);
        (*this->cache)[idx]->setMetadata(getTag(addr), true, false);
        cout << "block written at idx " << idx << endl;

        return true;
    }
    else
    {
        cout << "error opening the ram file\n";
        return false;
    }
}
int Controller::getByteOffset(const std::string& addr) const
{
    int m = log2(cache->nWords);
    string bo = addr.substr(ADDRESS_SIZE - 2, 2);
    return binToInt(bo);
}
int Controller::getWordOffset(const std::string& addr) const
{
    int m = log2(cache->nWords);
    string wo = addr.substr(ADDRESS_SIZE - 2 - m, m);
    return binToInt(wo);
}
void Controller::addInstruction(Instruction i)
{
    pendingInstructions.push(i);
}


