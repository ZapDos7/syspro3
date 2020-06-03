#ifndef _DHT_H_
#define _DHT_H_

#include "bb.h"
#include "record.h"
#include <cmath>

class aht
{
private:
    unsigned int size; //size of ht, h1 or h2
    bucket *table;     //the items
public:
    aht();
    aht(int hsize, int bsize);
    ~aht();
    unsigned int get_size();
    bucket *get_table();
    unsigned int ahash(std::string tbhashed); //a slightly better hash function compared to the simple HT
    void ainsert(record *r, bool isCountry);  //if isCountry == true, we hash a country, if false, we hash a disease
    void print_aht(bool isCountry);
    block *search(std::string s);
    void global_stats(date date1, date date2);
};

#endif