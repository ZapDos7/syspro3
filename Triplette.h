#ifndef _TRIPLETTE_H_
#define _TRIPLETTE_H_

#include <unistd.h>

using namespace std;

class Triplette
{
public:
    pid_t pid;

    int out; // aggregator to worker
    int in;  // worker to aggregator

    //methodoi
    Triplette(pid_t p);
    Triplette();
    virtual ~Triplette();
};



#endif