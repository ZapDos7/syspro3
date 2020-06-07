#include "StringArray.h"

StringArray::StringArray(int capacity) : Array(capacity)
{
}

StringArray::~StringArray()
{
}

void StringArray::print()
{
    for (int i = 0; i < size; i++)
    {
        cout << items[i] << " ";
    }

    cout << endl;
}

bool StringArray::has(std::string name) //an to name uparxei mes to array m
{
    for (int i = 0; i < this->size; i++)
    {
        if (this->items[i] == name)
            return true;
    }
    //else
    return false;
}