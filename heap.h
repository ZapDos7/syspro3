#ifndef _HEAP_H_
#define _HEAP_H_

#include <string>
#include <cmath>
#include "date.h"

//domi "max heap" oxi vasei array alla vasei dentrou, opws protathike sto eclass
class heap_node
{
private:
public:
    long int counter; //enas metritis == value pou sugkrinoume otan elegxoume me thn heapify. edw metrame posa krousmata exoume metrisei
    //std::string id;   //disease i country tou opoiou ta krousmata metrisame
    int apo;
    int ews;
    heap_node *left;
    heap_node *right;
    heap_node *parent;

    heap_node(int ilikia);
    heap_node();
    ~heap_node();
    bool isLeftNode();
    void print_heap_node();
    std::string get_node_data();
    void sink();
    void swim();
};

class heap
{
private:
public:
    heap_node *root; //max value, riza dentrou
    long int size;   //posa stoixeia exei mesa o swros. if size < k stis topk, print up to size.
    heap_node *last; //sto katwtero epipedo o deksioteros komvos pou einai != null

    heap(); //arxika empty
    ~heap();
    void print_heap(heap_node *hn); //kaleitai me 1o orisma to root
    heap_node *prev_last();         //epistrefei deikti ston proteleutaio node
    //void search(heap_node *hn, std::string s, heap_node **hnn);
    //void insert(std::string id0);                  //ws orisma pairnei ena string pou psaxnei kai ekei kanei insert
    void search(heap_node *hn, int age_to_be_compared, heap_node **hnn);
    void insert(int age_to_ins);
    /*heap_node **/ void pop_root(heap_node *res); //pop root, return new root after calling heapify
};
#endif