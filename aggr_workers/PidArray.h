#ifndef PIDARRAY_H
#define PIDARRAY_H

#include <unistd.h>
#include "Array.h"

class PidArray : public Array<pid_t>
{
public:
    PidArray(int numWorkers);
    virtual ~PidArray();

    virtual void print();
    void update_pid(pid_t, pid_t);
};

#endif