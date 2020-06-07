#ifndef _SUMM_H_
#define _SUMM_H

#include <string>
#include "record.h"

class Summ
{
private:
public:
    std::string diseaseName;
    int metr1; //0-20
    int metr2; //21-40
    int metr3; //41-60
    int metr4; //>60

    Summ();
    Summ(std::string dis);
    ~Summ();
    void add_record_metrites(record *rec);
    void add_record(record * rec);
};

#endif