#include "CacheLine.hpp"
#include <sstream>
#include <iomanip>
using namespace std;

void CacheLine::setMetadata(std::string tag, bool valid, bool dirty)
{
    this->tag = tag;
    this->valid = valid;
    this->dirty = dirty;
}
CacheLine::CacheLine(Cache* ptr_frame) 
    : tag(""), valid(false), dirty(false), ptr_frame(ptr_frame)
{
    setNWords();
    // cout << "resizing\n";
    data.resize(nWords);
    // cout << "resizing cmplete\n";
}

void CacheLine::invalidate()
{
    this->valid = false;
}
void CacheLine::setDirty()
{
    this->dirty = true;
}

string& CacheLine::operator [] (int idx)
{
    return data[idx];
}








void CacheLine::insertData(vector<string> data)
{
    for(int i = 0; i < nWords; i++)
    {
        this->data[i] = data[i];
    }
}

// ╭───────────────────────────────────╮
// │                                   │
// ╰───────────────────────────────────╯
// ├───────┤

string CacheLine::getLine() const
{
    int mdSize = getMetadataSize();
    // cout << "mdsize: " << mdSize << endl;

    stringstream ss;
    ss << "│" << setw(getTagSize()) << tag; ss << " ";
    ss << valid ? 1 : 0; ss << " ";
    ss << dirty ? 1 : 0; ss << " │ ";
    for(int i = 0; i < nWords; i++)
    {
        if(i == 0)
        {
            ss << setw(32) << data[i] << "│" << endl;

        }
        else
        {
            ss << "│";
            ss << string(mdSize, ' ');
            ss << " │ " << setw(32) << data[i] << "│" << endl;
        }
    }
    return ss.str();
}

int CacheLine::isDirty() const
{
    return dirty;
}
int CacheLine::isValid() const
{
    return valid;
}

int CacheLine::getMetadataSize() const
{
    // cout << "metadata size: " << getTagSize() + 4 << endl;
    return getTagSize() + 4;
}

int CacheLine::getSize() const
{
    // cout << "size: " << getMetadataSize() +  3 + 32 << endl;
    return getMetadataSize() +  3 + 32;
}
std::string CacheLine::getTag() const
{
    return tag;
}




// int main()
// {
//     cout << "runninng this\n";
//     CacheLine(nullptr);



//     return 0;
// }
