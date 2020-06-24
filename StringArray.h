#ifndef STRINGARRAY_H
#define STRINGARRAY_H

#include <iostream>

#include <string>

#include "Array.h"

using namespace std;

class StringArray : public Array<string>
{
public:
    StringArray(int capacity);
    StringArray();
    virtual ~StringArray();

    virtual void print();
    virtual void println();
    bool has(std::string name); //an to name uparxei mes to array m
};

#endif