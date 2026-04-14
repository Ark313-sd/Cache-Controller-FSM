#include "Cache.hpp"
#include "CacheLine.hpp"
#include <cmath>
using namespace std;



Cache::Cache(int linePower, int wordPower) 
    : nLines(pow(2,linePower)), nWords(pow(2,wordPower))
{
    line.resize(nLines);
    for(int i = 0; i < nLines; i++)
    {
        line[i] = new CacheLine(this);
    }
    this->line[0]->setWidth();
}

string Cache::constructLine(int length, const string& fillChar) const
{
    string line = "";
    for(int i = 0; i < length; i++)
    {
        line += fillChar;
    }
    return line;
}

void CacheLine::setNWords()
{
    this->nWords = ptr_frame->getNWords();
}

int Cache::getNWords() const
{
    return nWords;
}
int Cache::getNLines() const
{
    return nLines;
}
CacheLine* Cache::operator [] (int idx) const
{
    return line[idx];
}
// ├───────┤
// ├   vertical + right
// ┤   vertical + left
// ┬   horizontal + down
// ┴   horizontal + up
// ┼   full intersection
void Cache::printCache() const
{
    int mdSize = line[0]->getMetadataSize();

    cout << "╭" << constructLine(mdSize + 1, "─");
    cout << "┬" << constructLine(width - mdSize - 1 - 1, "─") << "╮" << endl;

    for(int i = 0; i < nLines; i++)
    {
        cout << line[i]->getLine();
        
        if(i != nLines - 1)
        {
            // cout << "├" << constructLine(width, "─") << "┤" << endl;
            cout << "├" << constructLine(mdSize + 1, "─");
            cout << "┼" << constructLine(width - mdSize - 1 - 1, "─") << "┤" << endl;
        }
    }
    cout << "╰" << constructLine(mdSize + 1, "─");
    cout << "┴" << constructLine(width - mdSize - 1 - 1, "─") << "╯" << endl;
}

int CacheLine::getTagSize() const
{
    return ADDRESS_SIZE - log2(this->ptr_frame->getNLines()) - 
        log2(this->ptr_frame->getNWords()) - 2; 
}

void CacheLine::setWidth()
{
    this->ptr_frame->getNLines();
    ptr_frame->width = getMetadataSize() + 3 + 32;
}

// ╭───────────────────────────────────╮
// │                                   │ 
// │───────────────────────────────────│
// │                                   │ 
// ╰───────────────────────────────────╯

// int main()
// {
    //     Cache cf;
//     cf.printFrame();
// }

