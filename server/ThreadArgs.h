
#ifndef THREADARGS_H
#define THREADARGS_H

#include <signal.h>
#include <condition_variable>

#include "Queue.h"


class ThreadArgs {
private:
    int socket_b;
public:
    int q, s, w, b, accept_client_fd, accept_worker_fd;
    Queue * queue;
    
    
    pthread_mutex_t socket_b_lock;
    pthread_mutex_t cout_lock;
    
    ThreadArgs(int q, int s, int w, int b, int accept_client_fd, int accept_worker_fd, Queue* queue) : q(q), s(s), w(w), b(b), accept_client_fd(accept_client_fd), accept_worker_fd(accept_worker_fd), queue(queue) {
        socket_b = -1;
        
        pthread_mutex_init(&socket_b_lock, NULL);
        pthread_mutex_init(&cout_lock, NULL);
    }
    
    ~ThreadArgs() {
        pthread_mutex_destroy(&socket_b_lock);
        pthread_mutex_destroy(&cout_lock);
    }

    void setSocketBufferSize(int newvalue) {
        pthread_mutex_lock(&socket_b_lock);
        socket_b = newvalue;
        pthread_mutex_unlock(&socket_b_lock);
    }
    
    int getSocketBufferSize() {
        pthread_mutex_lock(&socket_b_lock);
        int temp = socket_b;
        pthread_mutex_unlock(&socket_b_lock);
        return temp;
    }

};

#endif