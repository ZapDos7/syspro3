#ifndef _TREE_H_
#define _TREE_H_

#include "date.h"
#include "record.h"
#include "heap.h"

class tree_node
{
private:
public:
    date *d; //my key alla stin periptwsi pou einai diplotupa, exw list of these here
    record *rec;
    tree_node *left;
    tree_node *right;
    date * exitD;

    tree_node();
    tree_node(record *r);
    ~tree_node();
};

class tree
{
private:
public:
    tree_node *root;

    tree();
    ~tree();
    void in_order(tree_node *rt); //print in order ta elements tou tree
    tree_node *insert(tree_node *tr, record *r);
    tree_node *search(tree_node *tr, date d1);
    long int stats(tree_node *tr, date d1, date d2);
    long int statsCx(tree_node *tr, date d1, date d2, std::string countryName);    //gia global stats me country name
    long int statsExit(tree_node *tr, date d1, date d2);                           //idia me stats alla vasei exit date
    long int statsExitC(tree_node *tr, date d1, date d2, std::string countryName); //idia me exit alla & vasei country
    //void insert_to_heap_diseases(tree_node *tr, heap *swros);
    //void insert_to_heap_diseases_dates(tree_node *tr, heap *swros, date d1, date d2);
    //void insert_to_heap_countries(tree_node *tr, heap *swros);
    //void insert_to_heap_countries_dates(tree_node *tr, heap *swros, date d1, date d2);
    void insert_to_heap_diseases_countries_dates(tree_node *tr, heap *swros, date d1, date d2, std::string diseaseName);
};

#endif