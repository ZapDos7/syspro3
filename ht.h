#ifndef _HT_H
#define _HT_H_

#include <iostream>
#include <string>
#include "record.h"

class ht_item //functions like bucket
{
private:
    //mporw na kanw ta pedia private kai na valw getters kai setters, in future
public:
    record *rec;   //it's ID is the key!!
    ht_item *next; //node to next
    ht_item();
    ht_item(record *r);
    ~ht_item();
    void print_ht_item();
};

class ht
{
private:
    unsigned int size; //size of ht
    ht_item *table;    //the items
public:
    ht(unsigned int sz);
    ~ht();
    unsigned int get_size();
    ht_item *get_table();
    unsigned int hash(record r);      //the hash function, based on a record r(its ID, basically) returns an int.
    record *insert(record *r);        //mporei na epistrefei rec* gia na to parw ws orisma sta alla hash tables of satan
    ht_item *search(record *r);       //epistrefei to se poio index uparxei to record, else error msg
    unsigned int hash(std::string s); //vasei mono ID
    ht_item *search(std::string s);
    void print_ht();
};

#endif