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
    virtual ~StringArray();

    virtual void print();
    bool has(std::string name); //an to name uparxei mes to array m
};

#endif