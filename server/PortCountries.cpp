#include "PortCountries.h"

PortCountries::PortCountries() {}
PortCountries::PortCountries(int port, int capacity, std::string ip)
{
    this->my_port = port;
    this->my_countries.capacity = capacity;
    this->my_ip = ip;
}

PortCountries::~PortCountries() {}

void PortCountries::print_pc()
{
    std::cout << this->my_ip << " " << this->my_port << " ";
    for (int i = 0; i < this->my_countries.size; i++)
    {
        std::cout << this->my_countries.items[i] << " ";
    }
    std::cout << std::endl;
}