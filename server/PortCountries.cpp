#include "PortCountries.h"

PortCountries::PortCountries()
{
    this->my_port = 0;
    this->my_ip = "";
}
PortCountries::PortCountries(int port, int capacity, std::string ip) : my_countries(capacity)
{
    this->my_port = port;
    this->my_ip = ip;
}

PortCountries::~PortCountries() {}

void PortCountries::print_pc()
{
    fprintf(stdout, "%s %d ", this->my_ip.c_str(), this->my_port);
    this->my_countries.print();
    std::cout << std::endl;
}