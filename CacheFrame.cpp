#include "CacheFrame.hpp"
#include "CacheLine.hpp"
#include <cmath>
using namespace std;



CacheFrame::CacheFrame(int linePower, int wordPower) 
    : nLines(pow(2,linePower)), nWords(pow(2,wordPower))
{
    cout << "nlines: " << nLines << endl;
    line.resize(nLines);
    for(int i = 0; i < nLines; i++)
    {
        // cout << "constructing\n";
        line[i] = new CacheLine(this);
        // cout << "constructing complete\n";
    }
    this->line[0]->setWidth();
}

string CacheFrame::constructLine(int length, const string& fillChar) const
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

int CacheFrame::getNWords() const
{
    return nWords;
}
int CacheFrame::getNLines() const
{
    return nLines;
}
// ├───────┤
// ├   vertical + right
// ┤   vertical + left
// ┬   horizontal + down
// ┴   horizontal + up
// ┼   full intersection
void CacheFrame::printFrame() const
{
    int mdSize = line[0]->getMetadataSize();
    cout << "mdsize: " << mdSize << endl;

    cout << "╭" << constructLine(mdSize + 1, "─");
    cout << "┬" << constructLine(width - mdSize - 1 - 1, "─") << "╮" << endl;

    // cout << "mdSize = " << mdSize << endl;
    // cout << "left part = [" << constructLine(mdSize, "─") << "]" << endl;
    // cout << "length = " << constructLine(mdSize, "─").size() << endl;

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
    // cout << "tag size: " << ADDRESS_SIZE - log2(this->ptr_frame->getNLines()) - log2(this->ptr_frame->getNWords()) - 2 << endl; 
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
    //     CacheFrame cf;
//     cf.printFrame();
// }

