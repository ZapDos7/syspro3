#ifndef PAIR_H
#define PAIR_H

#include <string>
#include <unistd.h>

using namespace std;

class Pair
{
public:
    string country;
    pid_t pid;

    int out; // aggregator to worker
    int in;  // worker to aggregator

    //methodoi
    Pair(string country);
    Pair();
    virtual ~Pair();
};

#endif