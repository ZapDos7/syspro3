#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <string>
#include <iostream>
#include <errno.h>

using namespace std;

//from the book "Unix Programming" by M.J. Rochkind
ssize_t writeall(int fd, const void *buf, ssize_t nbyte);
ssize_t readall(int fd, void *buf, ssize_t nbyte);

class Communication
{
public:
    int b;

    Communication(int b);
    virtual ~Communication();

    char *createBuffer();
    void destroyBuffer(char *);

    void put(char *buf, string v);
    void put(char *buf, int v);

    void send(char *buf, int fd);
    void recv(char *buf, int fd);
};

#endif

//read(fd,&message_size,sizeof(int)) -> read message size (piazza)