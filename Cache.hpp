#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "config.hpp"
#include "CacheLine.hpp"



class Cache
{
    // cache info
    const int nLines;
    const int nWords;
    
    std::string constructLine(int length, const std::string& fillChar) const;
    friend class Controller;
    
public:
    Cache(int linePower=4, int wordPower=2);
    std::vector<CacheLine*> line;
    void printCache() const;
    int getNWords() const;
    int getNLines() const;
    
    CacheLine* operator [] (int idx) const;

    // box info
    int width;
};