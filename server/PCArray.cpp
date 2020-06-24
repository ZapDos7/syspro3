#include "PCArray.h"

PCArray::PCArray()
{
}

PCArray::PCArray(int capacity) : Array(capacity)
{
}
PCArray::~PCArray() {}

void PCArray::print()
{
    for (int i = 0; i < size; i++)
    {
        items[i]->print_pc();
    }
    fprintf(stdout, "\n");
}

int PCArray::has_pair(std::string ip, int port)
{
    for (int i = 0; i < this->size; i++)
    {
        if ((this->items[i]->my_ip == ip) && (this->items[i]->my_port == port))
        {
            return i;
        }
    }
    return -1;
}

int PCArray::has_country(std::string countryName)
{
    for (int i = 0; i < this->size; i++)
    {
        for (int j = 0; j < this->items[i]->my_countries.getSize(); j++)
        {
            if (this->items[i]->my_countries.items[j] == countryName)
            {
                return i;
            }
        }
    }
    return -1;
}