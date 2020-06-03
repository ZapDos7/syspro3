#include <iostream>
#include "ht.h"

using namespace std;

ht_item::ht_item()
{
    this->next = NULL;
    this->rec = NULL;
}

ht_item::ht_item(record *r)
{
    this->rec = r;
    this->next = NULL;
}

ht_item::~ht_item()
{
    if (this->rec != NULL)
    {
        delete this->rec;
    }
    if (this->next)
    {
        delete this->next;
    }
}

void ht_item::print_ht_item()
{
    std::cerr << this->rec->get_id() << " "
              << this->rec->get_fname() << " "
              << this->rec->get_lname() << " "
              << this->rec->get_disease() << " "
              << this->rec->get_country() << " "
              << this->rec->get_age() << " "
              << this->rec->get_entryDate().get_date_as_string() << " "
              << this->rec->get_exitDate().get_date_as_string() << std::endl;
}

//hash table

ht::ht(unsigned int sz)
{
    this->size = sz;
    this->table = new ht_item[size]; //size antikeimena ht_item poy den exoun tpt mesa tous basei tou empty consturctor tou ht_item dld ta pedia tous einai null automata
}

ht::~ht()
{
    //std::cout << "destroying ht \n";
    delete[] table;
}

unsigned int ht::get_size()
{
    return this->size;
}

ht_item *ht::get_table()
{
    return this->table;
}

unsigned int ht::hash(record r) //the hash function, based on a record r(its ID, basically) returns an int.
{
    //based on djb2
    unsigned int result = 5381;
    for (unsigned int i = 0; i < r.get_id().size(); i++)
    {
        result = 33 * result + (unsigned char)r.get_id()[i];
    }
    return result % this->size;
}

unsigned int ht::hash(std::string s) //the hash function, based on a record r(its ID, basically) returns an int.
{
    //based on djb2
    unsigned int result = 5381;
    for (unsigned int i = 0; i < s.size(); i++)
    {
        result = 33 * result + (unsigned char)s[i];
    }
    return result % this->size;
}

record *ht::insert(record *r) //mporei na epistrefei rec* gia na to parw ws orisma sta alla hash tables of satan
{                             //antigrafw ta stoixeia tou r sto dynamic record pou tha mpei sto ht mas
    unsigned int where = hash(*r);
    //paw sto table[where] pou einai ena ht_item, oxi ht_item*
    if (table[where].rec == NULL) //den exw idi record edw
    {
        table[where].rec = new record(*r);
        //table[where].rec = r;
        //std::cerr << table[where].rec->get_id();
        return table[where].rec;
    }
    else if (table[where].next == NULL) //exw record alla den exw next
    {
        if (table[where].rec->get_id() == r->get_id())
        {
            //std::cerr << "Dublicate record ID. Fix your dataset and try again.\n";
            std::cerr << "error\n";
            return NULL;
        }
        else //grafw ston next moy
        {
            table[where].next = new ht_item;
            table[where].next->rec = new record(*r);
            //std::cerr << table[where].rec->get_id();
            return table[where].next->rec;
        }
    }
    else //uparxei kai next
    {
        ht_item *temp = &table[where];
        if (temp->rec->get_id() == r->get_id())
        {
            //std::cerr << "Dublicate record ID. Fix your dataset and try again(1).\n";
            std::cerr << "error\n";
            return NULL;
        }
        else
        {
            while (temp->next != NULL) //psaxnw na vrw to next null gia na paw sto telos tis listas autou tou stoixeiou tou ht
            {
                temp = temp->next;                      //paw ston epomeno mou kai elegxw an uparxei idi to ID ekei
                if (temp->rec->get_id() == r->get_id()) //an edw mesa vrw to ID tou r, exit
                {
                    //std::cerr << "Dublicate record ID. Fix your dataset and try again(2).\n";
                    std::cerr << "error\n";
                    return NULL;
                }
            }
            //else, exw brei to telos twn buckets kai paw kai grafw
            temp->next = new ht_item;
            temp->next->rec = new record(*r);
            //std::cerr << table[where].rec->get_id();
            return temp->next->rec;
        }
    }
}

ht_item *ht::search(record *r)
{
    unsigned int where = hash(*r);
    if (table[where].rec == NULL) //den exw kanena record edw
    {
        //std::cerr << "This record isn't in my hash table\n";
        return NULL;
    }
    else //den einai null auto to .rec ara exw eggrafi alla isws den einai i diki m => elegxw ID
    {
        ht_item *now = &(table[where]);
        while (now->next != NULL) //iterate through these buckets opou exw collision gia na vrw an uparxei to r
        {
            if (now->rec->get_id() == r->get_id())
            {
                //std::cerr << "Found!\n";
                return now;
            }
            else
            {
                now = now->next; //pame sto epomeno
            }
        }
    }
    return NULL;
}

ht_item *ht::search(std::string s)
{
    unsigned int where = hash(s);
    if (table[where].rec == NULL) //den exw kanena record edw
    {
        //std::cerr << "This record isn't in my hash table\n";
        return NULL;
    }
    else //den einai null auto to .rec ara exw eggrafi alla isws den einai i diki m => elegxw ID
    {
        ht_item *now = &(table[where]);

        if (now->rec == NULL)
        {
            return NULL;
        }
        if (now->rec->get_id() == s)
        {
            return now;
        }

        while (now->next != NULL) //iterate through these buckets opou exw collision gia na vrw an uparxei to r
        {
            now = now->next;

            if (now->rec->get_id() == s)
            {
                //std::cerr << "Found!\n";
                return now;
            }
        }
    }
    return NULL;
}

void ht::print_ht()
{
    for (unsigned int i = 0; i < size; i++)
    {
        if (table[i].rec != NULL)
        {
            table[i].print_ht_item();
            ht_item *temp = table[i].next;
            while (temp != NULL)
            {
                temp->print_ht_item();
                temp = temp->next;
            }
        }
    }
}