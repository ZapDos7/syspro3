#include <string>
#include <stddef.h>
#include <iostream>
#include <cstring>

#include "date_format.h"

using namespace std;

bool date_format(std::string str) //elegxei oti ena string einai stin morfi XX-YY-ZZZZ
{
    std::string a0 = "";
    //elegxw oti ta comms 1, 2 einai ok gia na perastoun ws orismata ston constructor twn dates
    for (unsigned int j = 0; j < str.length(); j++) //trekse to mikos tou orismatos
    {
        if ((str[j] - '0' > -1) && (str[j] - '0' < 10)) //it's a digit
        {
            a0 = a0 + str[j];
        }
        else if (str[j] == '-')
        {
            a0 = a0 + str[j];
        }
        else
        {
            //std::cerr << "Type properly(1).\n";	 //continue to next command
            std::cout << "error\n";
        }
    }
    char *cstr = new char[a0.length() + 1]; //auto 8a kanw tokenize
    strcpy(cstr, a0.c_str());               //copy as string to a0 sto cstr
    char *pch;
    const char delim[2] = "-";
    pch = strtok(cstr, delim);
    short unsigned int counter_a = 0;
    while (pch != NULL)
    {
        counter_a++;
        pch = strtok(NULL, delim);
    }
    delete[] cstr;
    if (counter_a != 3)
    {
        return false;
    }
    else
    {
        return true;
    }
}