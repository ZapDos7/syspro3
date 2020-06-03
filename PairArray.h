#ifndef PAIRARRAY_H
#define PAIRARRAY_H

#include "Array.h"
#include "Pair.h"

class PairArray : public Array<Pair>
{
public:
    PairArray(int capacity);

    virtual void print();
    void print_lc(); //print list countries
};

#endif