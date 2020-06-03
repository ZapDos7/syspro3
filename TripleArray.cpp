#include "TripleArray.h"

TripleArray::TripleArray(int capacity) : Array(capacity){}
TripleArray::~TripleArray(){}


void TripleArray::print()
{
    for (int i = 0; i < size; i++)
    {
        cout << items[i].pid << " " << items[i].out << " " << items[i].in << endl;
    }
}