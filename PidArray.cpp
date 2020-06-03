#include "PidArray.h"

#include <iostream>

using namespace std;

PidArray::PidArray(int numWorkers) : Array(numWorkers)
{
}

PidArray::~PidArray()
{
}

void PidArray::print()
{
    for (int i = 0; i < size; i++)
    {
        cout << items[i] << " ";
    }
    cout << endl;
}