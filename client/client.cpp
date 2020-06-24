/*	
Ioanna Zapalidi, sdi1400044	
System Programming Project #3, Spring 2020	
 */
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include "../Communication.h"
#include "../StringArray.h"
#include "ThreadArgsC.h"

static pthread_barrier_t barrier; //gia tautoxroni ekkinisi olwn twn threads
static pthread_mutex_t mymutex; //gia pthreadargs->array.items[i]

void *ask(void *args) //thread asks query to server & then receives answer by a worker
{
    ThreadArgsC *pthreadargs = (ThreadArgsC *)args;
    char token = 'C';

    fprintf(stdout, "I am thread %ld and I am starting now now ...\n", pthread_self());

    pthread_barrier_wait(&barrier);

    cout << "barrier broken, threads start! \n";

    for (int i = 0; i < pthreadargs->array.getSize(); i++)
    {
        int accept_server_fd = Communication::create_connecting_socket(pthreadargs->sip, pthreadargs->sp);

        int check = write(accept_server_fd, &token, sizeof(token));
        if (check < 0)
        {
            fprintf(stderr, "write C");
            exit(EXIT_FAILURE);
        }
        int b;

        if (read(accept_server_fd, &b, sizeof(b)) <= 0) //lambanw to b gia na ftiaksw ton communicator
        {
            fprintf(stderr, "network communication corruption detected \n");
            exit(1);
        }
        else
        {
            printf("b =  %d \n", b);
        }

        if (accept_server_fd > 0)
        {
            Communication communicator(b);

            char *buf = communicator.createBuffer();
            communicator.put(buf, pthreadargs->array.items[i].c_str());
            communicator.send(buf, accept_server_fd);
            communicator.recv(buf, accept_server_fd);

            pthread_mutex_lock(&mymutex);
            fprintf(stdout, "\t>>SENT: %lu: command: '%s'\n", pthread_self(), pthreadargs->array.items[i].c_str());
            fprintf(stdout, "\t>>RECV: %lu: '%s'\n", pthread_self(), buf);
            pthread_mutex_unlock(&mymutex);

            communicator.destroyBuffer(buf);

            close(accept_server_fd);
        }
        else
        {
            fprintf(stdout, "\t>>FAIL: %lu: b=%d command: '%s'\n", pthread_self(), b, pthreadargs->array.items[i].c_str());
        }
    }

    pthread_exit(NULL);

    return 0;
}

int main(int argc, char const *argv[])
{
    char query_file[256]; //query file name
    int numThreads = -1;  //numthreads
    uint16_t sp = -1;     //server port
    char sip[256];        //serverIP

    for (int i = 0; i < argc; i++)
    {
        if (strcmp("-q", argv[i]) == 0)
        {
            strcpy(query_file, argv[i + 1]);
        }
        if (strcmp("-numThreads", argv[i]) == 0 || strcmp("-w", argv[i]) == 0)
        {
            numThreads = atoi(argv[i + 1]);
        }
        if (strcmp("-sp", argv[i]) == 0)
        {
            sp = (uint16_t)atoi(argv[i + 1]);
        }
        if (strcmp("-sip", argv[i]) == 0)
        {
            strcpy(sip, argv[i + 1]);
        }
    }
    if ((sp < 0) || (numThreads < 0))
    {
        fprintf(stderr, "critical error: arguements\n");
        exit(EXIT_FAILURE);
    }

    if (numThreads > 50)
    {
        fprintf(stderr, "Number of threads should be no more than 50\n");
        exit(0);
    }

    pthread_barrier_init(&barrier, NULL, numThreads);
    pthread_mutex_init(&mymutex, NULL);

    std::ifstream dataset(query_file);
    std::string line;
    int posa_commands = 0;
    StringArray entoles; // "cached" file

    //arxika aplws stelnw 1-1 ston server
    while (std::getline(dataset, line))
    {
        if (line.length() > 0)
        {
            entoles.insert(line);
            posa_commands++;
        }
    }

    //entoles.println();

    //threadify process of asking questions
    ThreadArgsC **threadargs = new ThreadArgsC *[posa_commands];

    for (int i = 0; i < numThreads; i++)
    {
        threadargs[i] = new ThreadArgsC(query_file, numThreads, sp, sip);
    }

    for (int i = 0; i < posa_commands; i++)
    {
        threadargs[i % numThreads]->array.insert(entoles.items[i]);
    }

    pthread_t asking[numThreads];

    for (int i = 0; i < numThreads; i++)
    {
        pthread_create(&asking[i], NULL, ask, (void *)threadargs[i]);
    }

    fprintf(stderr, "Main client thread waiting for children client slaves to exit ... \n");

    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(asking[i], NULL);
    }

    for (int i = 0; i < posa_commands; i++)
    {
        delete threadargs[i];
    }

    delete[] threadargs;

    pthread_barrier_destroy(&barrier);
    pthread_mutex_destroy(&mymutex);

    return 0;
}
