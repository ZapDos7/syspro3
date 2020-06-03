#include "PairArray.h"
#include "Array.h"

#include <iostream>

using namespace std;

PairArray::PairArray(int capacity) : Array(capacity)
{
}

void PairArray::print()
{
    for (int i = 0; i < size; i++)
    {
        cout << items[i].country << " " << items[i].pid << " " << items[i].out << " " << items[i].in << endl;
    }
}

void PairArray::print_lc()
{
    for (int i = 0; i < size; i++)
    {
        cout << items[i].country << " " << items[i].pid << endl;
    }
}