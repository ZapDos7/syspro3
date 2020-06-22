
#ifndef THREADARGSC_H
#define THREADARGSC_H

#include <signal.h>
#include <condition_variable>

class ThreadArgsC
{
private:
public:
    char query_file[256]; //query file name
    int numThreads = -1;  //numthreads - is this needed?
    uint16_t sp = -1;     //server port
    char sip[256];        //serverIP

    pthread_mutex_t query_file_lock;
    pthread_mutex_t cout_lock; //used gia cout an den kanw dld fprintf

    ThreadArgsC(char *qf, int nt, uint16_t sp1, char *sip1)
    {
        strcpy(query_file, qf);
        numThreads = nt;
        sp = sp1;
        strcpy(sip, sip1);

        pthread_mutex_init(&query_file_lock, NULL);
        pthread_mutex_init(&cout_lock, NULL);
    }

    ~ThreadArgsC()
    {
        pthread_mutex_destroy(&query_file_lock);
        pthread_mutex_destroy(&cout_lock);
    }
};

#endif