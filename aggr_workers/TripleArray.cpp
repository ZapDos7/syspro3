#include "TripleArray.h"

TripleArray::TripleArray(int capacity) : Array(capacity) {}
TripleArray::~TripleArray() {}

void TripleArray::print()
{
    for (int i = 0; i < size; i++)
    {
        cout << items[i].pid << " " << items[i].out << " " << items[i].in << endl;
    }
}

void TripleArray::swap(pid_t pid_prin, Triplette meta)
{
    for (int i = 0; i < this->size; i++)
    {
        if (this->items[i].pid == pid_prin)
        {
            this->items[i] = meta;
        }
    }
}