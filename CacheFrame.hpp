#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "CacheLine.hpp"

const int ADDRESS_SIZE = 16;

class CacheFrame
{
    // cache info
    const int nLines;
    const int nWords;
    
    
    std::string constructLine(int length, const std::string& fillChar) const;
    
    public:
    CacheFrame(int linePower=4, int wordPower=2);
    void printFrame() const;
    int getNWords() const;
    int getNLines() const;
    
    std::vector<CacheLine*> line;

    // box info
    int width;
};