#ifndef SUMMARRAY_H
#define SUMMARRAY_H

#include "Array.h"
#include "Summ.h"

class SummArray : public Array<Summ>
{
public:
    SummArray(int capacity);

    virtual void print();
    bool has(std::string name);
    void insert_rec(record * rec);
};

#endif