#include "StringArray.h"

StringArray::StringArray(int capacity) : Array(capacity)
{
}
StringArray::StringArray() : Array(10)
{
}

StringArray::~StringArray()
{
}

void StringArray::print()
{
    for (int i = 0; i < size; i++)
    {
        fprintf(stdout, "%s ", items[i].c_str());
    }
    fprintf(stdout, "\n");
}

void StringArray::println()
{
    for (int i = 0; i < size; i++)
    {
        fprintf(stdout, "%s \n", items[i].c_str());
    }
    fprintf(stdout, "\n");
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