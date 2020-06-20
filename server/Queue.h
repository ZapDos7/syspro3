
#ifndef QUEUE_H
#define QUEUE_H

class Queue {
public:
    Queue(int POOL_SIZE);
    virtual ~Queue();
    
    void place(int data);
    int obtain();
private:
    int * data;
    int start, end, count, POOL_SIZE;
};

#endif