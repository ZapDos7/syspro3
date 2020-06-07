#include "Summ.h"

Summ::Summ()
{
    this->diseaseName = "";
    this->metr1 = 0;
    this->metr2 = 0;
    this->metr3 = 0;
    this->metr4 = 0;
}
Summ::Summ(std::string dis)
{
    this->diseaseName = dis;
    this->metr1 = 0;
    this->metr2 = 0;
    this->metr3 = 0;
    this->metr4 = 0;
}

Summ::~Summ()
{
}

void Summ::add_record_metrites(record * rec)
{
    int ilikia = rec->get_age();
    if ((ilikia < 20)&&(ilikia > 0)) metr1++;
    else if ((ilikia < 40)&&(ilikia > 21)) metr2++;
    else if ((ilikia < 60)&&(ilikia > 41)) metr3++;
    else if ((ilikia < 120)&&(ilikia > 60)) metr4++;
    return;
}

void Summ::add_record(record * rec)
{
    this->diseaseName = rec->get_disease();
    int ilikia = rec->get_age();
    if ((ilikia < 20)&&(ilikia > 0)) metr1++;
    else if ((ilikia < 40)&&(ilikia > 21)) metr2++;
    else if ((ilikia < 60)&&(ilikia > 41)) metr3++;
    else if ((ilikia < 120)&&(ilikia > 60)) metr4++;
    return;
}