
#ifndef QUEUE_H
#define QUEUE_H

class Queue
{
private:
    int *data;
    int start, end, count, POOL_SIZE;

public:
    Queue(int POOL_SIZE);
    virtual ~Queue();

    void place(int data);
    int obtain();
};

#endif