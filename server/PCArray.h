#ifndef _PORT_COUNTRIES_ARRAY_H
#define _PORT_COUNTRIES_ARRAY_H

#include "PortCountries.h"
#include "../Array.h"

class PCArray : public Array<PortCountries>
{
private:
public:
    PCArray(int capacity);
    PCArray(); //def
    ~PCArray();
    virtual void print();

    int has_pair(std::string ip, int port); //-1 an den uparxei, else to i tis thesis opou uparxei
    int has_country(std::string countryName); //-1 an den tin exei, else to index tis thesis opou uparxei
};


#endif