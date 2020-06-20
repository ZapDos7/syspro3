#include <signal.h>
#include <condition_variable>

#include "Queue.h"

//vasei paradeigmatwn mathimatos

static pthread_mutex_t mtx;
static pthread_cond_t cond_nonempty;
static pthread_cond_t cond_nonfull;

Queue::Queue(int POOL_SIZE) : start(0), end(-1), count(0), POOL_SIZE(POOL_SIZE)
{
    data = new int[POOL_SIZE];

    pthread_mutex_init(&mtx, 0);
    pthread_cond_init(&cond_nonempty, 0);
    pthread_cond_init(&cond_nonfull, 0);
}

Queue::~Queue()
{
    delete[] data;

    pthread_cond_destroy(&cond_nonempty);
    pthread_cond_destroy(&cond_nonfull);
    pthread_mutex_destroy(&mtx);
}

void Queue::place(int data)
{
    pthread_mutex_lock(&mtx);
    while (this->count >= POOL_SIZE)
    {
        pthread_cond_wait(&cond_nonfull, &mtx);
    }
    this->end = (this->end + 1) % POOL_SIZE;
    this->data[this->end] = data;
    this->count++;
    pthread_cond_signal(&cond_nonempty);
    pthread_mutex_unlock(&mtx);
}

int Queue::obtain()
{
    int data = 0;
    pthread_mutex_lock(&mtx);
    while (this->count <= 0)
    {
        pthread_cond_wait(&cond_nonempty, &mtx);
    }
    data = this->data[this->start];
    this->start = (this->start + 1) % POOL_SIZE;
    this->count--;
    pthread_cond_signal(&cond_nonfull);
    pthread_mutex_unlock(&mtx);
    return data;
}
