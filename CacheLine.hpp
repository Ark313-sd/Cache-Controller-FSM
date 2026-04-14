#pragma once
#include <iostream>
#include <string>
#include <vector>



class CacheFrame;

class CacheLine
{
    std::string tag;
    bool valid;
    bool dirty;
    int nWords;
    std::vector<std::string> data;
    int getMetadataSize() const;
    void setDirty();
    int getTagSize() const;
    int getSize() const;
    
    void setNWords();
    CacheFrame *ptr_frame;
    void setWidth();

    friend class CacheFrame;
public:
    CacheLine(CacheFrame* ptr_frame);
    void setMetadata(std::string tag, bool valid=true, bool dirty=false);
    void insertData(std::vector<std::string> data);
    std::string getLine() const;
    void invalidate();

};