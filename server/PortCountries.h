#ifndef _PORTNUM_COUNTRIES_H
#define _PORTNUM_COUNTRIES_H

#include "../StringArray.h"

//sundiazei port num & StringArray apo xwres

class PortCountries
{
private:
public:
    StringArray my_countries;
    int my_port;
    std::string my_ip;

    PortCountries();
    PortCountries(int port, int capacity, std::string ip);
    ~PortCountries();

    void print_pc();
};

#endif