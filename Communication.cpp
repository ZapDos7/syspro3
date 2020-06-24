#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h> /* sockets */
#include <netdb.h>    /* gethostbyaddr */

#include "Communication.h"

ssize_t writeall(int fd, const void *buf, ssize_t nbyte)
{
    ssize_t nwritten = 0, n;

    do
    {
        if ((n = write(fd, &((const char *)buf)[nwritten], nbyte - nwritten)) == -1)
        {
            if (errno == EINTR)
                continue;
            else
            {
                perror("writeall \n");
                return -1;
            }
        }
        nwritten += n;
    } while (nwritten < nbyte);

    return nwritten;
}

ssize_t readall(int fd, void *buf, ssize_t nbyte)
{
    ssize_t nread = 0, n;

    do
    {
        if ((n = read(fd, &((char *)buf)[nread], nbyte - nread)) == -1)
        {
            if (errno == EINTR)
            {
                return 0;
            }
            else
            {
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

//Communication::Communication() : b(COMMUNICATOR_DEFAULT_SIZE) {
//}

Communication::Communication(int b) : b(b)
{
    //cout << "communication created with b = " << b << endl;
}

Communication::~Communication()
{
}

char *Communication::createBuffer()
{
    return new char[b]();
}

void Communication::destroyBuffer(char *buf)
{
    delete[] buf;
}

void Communication::put(char *buf, string v)
{
    strcpy(buf, v.c_str()); //returns "buf" if ok
}

void Communication::put(char *buf, int v)
{
    string s = to_string(v);
    put(buf, s);
}

void Communication::send(char *buf, int fd)
{
    //int check = write(fd, buf, b);
    int check = writeall(fd, buf, b);
    if (check == -1)
    {
        cerr << "errno is " << errno << " ";
        perror("communication::send");
    }
    else if (check < b)
    {
        cerr << "Didn't write enough bytes\n";
        cerr << "errno is " << errno << " ";
        perror("communication::send");
    }
}

void Communication::send(char buf, int fd)
{
    //int check = write(fd, buf, b);
    int check = write(fd, &buf, sizeof(buf));
    if (check == -1)
    {
        cerr << "errno is " << errno << " ";
        perror("communication::send");
    }
    else if (check == 0)
    {
        cerr << "Didn't write enough bytes\n";
        cerr << "errno is " << errno << " ";
        perror("communication::send");
    }
}

void Communication::recv(char *buf, int fd)
{
    //int check = read(fd, buf, b);
    int check = readall(fd, buf, b);
    if (check == -1 && errno == EINTR)
    {
        return;
    }
    if (check == -1) //or if check!=n
    {
        cerr << "errno is " << errno << " ";
        perror("communication::recv");
    }
    else if (check < b)
    {
        if (check == 0)
        {
            perror("communication::recv");
            cerr << "Communication closed by peer \n";
        }
        else
        {
            cerr << "Didn't read enough bytes\n";
            cerr << "errno is " << errno << " ";
            perror("communication::recv");
        }
    }
}

void Communication::send(int buf, int fd)
{
    //int check = write(fd, buf, b);
    int check = write(fd, &buf, sizeof(buf));
    if (check == -1)
    {
        cerr << "errno is " << errno << " ";
        perror("communication::send");
    }
    else if (check == 0)
    {
        cerr << "Didn't write enough bytes\n";
        cerr << "errno is " << errno << " ";
        perror("communication::send");
    }
}

void Communication::recv(int buf, int fd)
{
    //int check = read(fd, buf, b);
    int check = read(fd, &buf, sizeof(buf));
    if (check == -1 && errno == EINTR)
    {
        return;
    }
    if (check == -1) //or if check!=n
    {
        cerr << "errno is " << errno << " ";
        perror("communication::recv");
    }
    else if (check == 0)
    {
        perror("communication::recv");
        cerr << "Communication closed by peer \n";
    }
}

void Communication::recv(char buf, int fd)
{
    //int check = read(fd, buf, b);
    int check = read(fd, &buf, sizeof(buf));
    if (check == -1 && errno == EINTR)
    {
        return;
    }
    if (check == -1) //or if check!=n
    {
        cerr << "errno is " << errno << " ";
        perror("communication::recv");
    }
    else if (check == 0)
    {
        perror("communication::recv");
        cerr << "Communication closed by peer \n";
    }
}

int Communication::create_listening_socket(uint16_t &port, int backlog)
{
    struct sockaddr_in address;
    socklen_t address_l = sizeof(address);
    int fd;

    // creating socket fd
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&address, address_l) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(fd, backlog) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if (port == 0)
    {
        if (getsockname(fd, (struct sockaddr *)&address, &address_l) == -1)
        {
            perror("getsockname");
        }
        else
        {
            port = ntohs(address.sin_port);
        }
    }

    return fd;
}

int Communication::create_listening_socket(uint16_t &port)
{
    struct sockaddr_in address;
    socklen_t address_l = sizeof(address);
    int fd;

    // creating socket fd
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&address, address_l) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(fd, 100) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if (port == 0)
    {
        if (getsockname(fd, (struct sockaddr *)&address, &address_l) == -1)
        {
            perror("getsockname");
        }
        else
        {
            port = ntohs(address.sin_port);
        }
    }

    return fd;
}

int Communication::create_connecting_socket(const char *ip, uint16_t &port)
{
    struct sockaddr_in server; //edw 8a m leei o server ti thelei
    socklen_t server_addr_l = sizeof(server);
    struct hostent *rem;
    int fd;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("\n Socket creation error1 \n");
        return -1;
    }

    if ((rem = gethostbyname(ip)) == NULL)
    {
        herror("gethostbyname");
        return -1;
    }

    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (connect(fd, (struct sockaddr *)&server, server_addr_l) < 0)
    {
        perror("Connection Failed");
        return -1;
    }

    return fd;
}

int accept_socket(int fd)
{
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    int new_socket;
    if ((new_socket = accept(fd,  (struct sockaddr *)&address, &addrlen))<0)
    {
        perror("accept failed");
        return -1;
    }
    //else
    return new_socket;
}