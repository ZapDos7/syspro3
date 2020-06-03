#include <cstring>
#include <unistd.h>

#include "Communication.h"

ssize_t writeall(int fd, const void *buf, ssize_t nbyte) {
    ssize_t nwritten = 0, n;

    do {
        if ((n = write(fd, &((const char *) buf)[nwritten], nbyte - nwritten)) == -1) {
            if (errno == EINTR)
                continue;
            else {
                perror("writeall \n");
                return -1;
            }
        }
        nwritten += n;
    } while (nwritten < nbyte);

    return nwritten;
}

ssize_t readall(int fd, void *buf, ssize_t nbyte) {
    ssize_t nread = 0, n;

    do {
        if ((n = read(fd, &((char *) buf)[nread], nbyte - nread)) == -1) {
            if (errno == EINTR) {
                return 0;
            } else {
                perror("Readall \n");
                return -1;
            }
        }
        if (n == 0)
            return nread;
        nread += n;
    } while (nread < nbyte);
    return nread;
}

Communication::Communication(int b) : b(b) {
}

Communication::~Communication() {
}

char *Communication::createBuffer() {
    return new char[b]();
}

void Communication::destroyBuffer(char *buf) {
    delete[] buf;
}

void Communication::put(char *buf, string v) {
    strcpy(buf, v.c_str()); //returns "buf" if ok
}

void Communication::put(char *buf, int v) {
    string s = to_string(v);
    put(buf, s);
}

void Communication::send(char *buf, int fd) {
    //int check = write(fd, buf, b);
    int check = writeall(fd, buf, b);
    if (check == -1) {
        cerr << "errno is " << errno << " ";
        perror("communication::send");
    } else if (check < b) {
        cerr << "Didn't write enough bytes\n";
        cerr << "errno is " << errno << " ";
        perror("communication::send");
    }
}

void Communication::recv(char *buf, int fd) {
    //int check = read(fd, buf, b);
    int check = readall(fd, buf, b);
    if (check == -1 && errno == EINTR) {
        return;
    }
    if (check == -1) //or if check!=n
    {
        cerr << "errno is " << errno << " ";
        perror("communication::recv");
    } else if (check < b) {
        if (check == 0) {
            perror("communication::recv");
            cerr << "Communication closed by peer \n";
        } else {
            cerr << "Didn't read enough bytes\n";
            cerr << "errno is " << errno << " ";
            perror("communication::recv");
        }
    }
}