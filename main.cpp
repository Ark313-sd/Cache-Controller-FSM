#include <iostream>
#include <vector>
#include "CacheFrame.hpp"
using namespace std;




int main()
{
    cout << "Hello\n";
    CacheFrame cf;
    cout << "width: " << cf.width << endl;

    cf.line[0]->setMetadata("00000000", true, false);
    vector<string> data = {"10110011100011110000111100110011", "0000", "1111", "1010"};
    cf.line[0]->insertData(data);
    // cout << cf.line[0]->getLine();

    cf.printFrame();




    return 0;
}