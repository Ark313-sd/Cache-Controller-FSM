#include "config.hpp"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
using namespace std;

void createRAM()
{
    ofstream file("ram.txt", ios::out);
    if(file.is_open() && !file.fail())
    {
        srand(time(0));
        for(long long int i = 0; i < TOTAL_BYTES; i++)
        {
            string bitString = "";
            for(int j = 0; j < 8; j++)
            {
                int randBit = rand() % 2;
                bitString += to_string(randBit);
            }

            file << "x" << uppercase << hex << setw(4) << setfill('0') << i << " " << bitString << '\n';        
        }
        file.close();
    }
}