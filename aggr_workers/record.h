#ifndef _RECORD_H_
#define _RECORD_H_

#include <string>
#include <cstring>
#include <iostream>
#include "date.h"

class record
{
private:
    std::string id;      //unique
    std::string fname;   //oxi kena, mono grammata
    std::string lname;   //oxi kena, mono grammata
    std::string disease; //string w/ letters, numbers kai "-"
    std::string country; //oxi kena, mono grammata
    date entryD;         /*entryDate:  DD-MM-YYYY */
    date exitD;          /*exitDate:   DD-MM-YYYY or "-" (den exei eksitirio akoma o asthenis)*/
    int age;

public:
    record();
    record(std::string line);
    record(record &r);
    ~record();
    std::string get_id();
    std::string get_fname();
    std::string get_lname();
    std::string get_disease();
    std::string *get_diseasePtr();
    std::string get_country();
    std::string *get_countryPtr();
    date get_entryDate();
    date *get_entryDatePtr();
    date get_exitDate();
    date *get_exitDatePtr();
    int get_age();
    void set_id(std::string id_to_be);
    void set_fname(std::string fn);
    void set_lname(std::string ln);
    void set_disease(std::string dis);
    void set_country(std::string cntr);
    void set_entryD(std::string enD);
    void set_exitD(std::string exD);
    void set_age(int a);
    void print_record();
};

#endif
