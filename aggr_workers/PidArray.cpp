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

void PidArray::update_pid(pid_t proc_prin, pid_t proc_meta)
{
    for (int i = 0; i < size; i++)
    {
        if (items[i]==proc_prin)
        {
            items[i] = proc_meta;
        }
    }
    return;
}