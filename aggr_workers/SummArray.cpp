#include "SummArray.h"
#include "Array.h"

#include <iostream>

using namespace std;

SummArray::SummArray(int capacity) : Array(capacity)
{
}

void SummArray::print()
{
    for (int i = 0; i < size; i++)
    {
        cout << items[i].diseaseName << " " << items[i].metr1 << " " << items[i].metr2 << " " << items[i].metr3 << " " << items[i].metr4 << "\n";
    }
}

bool SummArray::has(std::string name)
{
    for (int i = 0; i < this->size; i++)
    {
        if (this->items[i].diseaseName == name)
        {
            return true;
        }
    }
    return false;
}

void SummArray::insert_rec(record *rec)
{
    if (this->has(rec->get_disease())) //uparxei idi to disease ara update metirtes
    {
        for (int i = 0; i < this->size; i++)
        {
            if (this->items[i].diseaseName == rec->get_disease())
            {
                this->items[i].add_record_metrites(rec);
            }
        }
    }
    else //den uparxei ara kanw neo Summ mes to array & vazw ta data data
    {
        if (size < capacity)
        {
            Summ item; //(rec->get_disease());
            item.add_record(rec);
            items[size] = item;
            size++;
        }
        else
        {
            Summ *buf = new Summ[capacity * 2];
            for (int i = 0; i < capacity; i++)
            {
                buf[i] = items[i];
            }
            delete[] items;
            items = buf;

            capacity = 2 * capacity;
            Summ item;            //(rec->get_disease());
            item.add_record(rec); //Summ item(rec->get_disease());
            items[size] = item;
            size++;
        }
    }
}