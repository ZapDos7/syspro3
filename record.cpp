#include "record.h"

using namespace std;

record::record() {}

record::record(string line) //thewrw oti dineis sxetika swsto record kai xeirizomai merika ksekathara lathi
{
    char *cstr = new char[line.length() + 1]; //auto 8a kanw tokenize
    strcpy(cstr, line.c_str());               //copy as string to line sto cstr
    char *pch;
    const char delim[5] = " \r\n\t"; //\0
    pch = strtok(cstr, delim);
    short unsigned int counter = 0;
    char *parts[8]; //+1 gia age! apo tin proigoumeni ergasia
    while (pch != NULL)
    {
        parts[counter] = pch;
        //std::cerr << pch << "\n";
        if (counter == 8) //ama dw8ei o,ti na nai gia record, yeet -> this saves us from a seg fault an dw8ei dld record swsta kai meta akura strings
        {
            pch = strtok(NULL, delim);
            break;
        }
        counter++;
        pch = strtok(NULL, delim);
    }
    //std::cerr << counter << "\n";
    for (unsigned int i = 0; i < counter; i++)
    {
        string tmp(parts[i]);
        //std::cerr << tmp << '\n';
        if (i == 0)
            id = tmp;
        else if (i == 1)
            fname = tmp;
        else if (i == 2)
            lname = tmp;
        else if (i == 3)
            disease = tmp;
        else if (i == 4)
            country = tmp;
        else if (i == 5)
            age = stoi(tmp);
        else if (i == 6)
        {
            date tmpD(tmp);
            entryD.set_day(tmpD.get_day());
            entryD.set_month(tmpD.get_month());
            entryD.set_year(tmpD.get_year());
        }
        else if (i == 7)
        {
            //std::cerr << tmp << "\n";
            if ((tmp == "-") || (counter == 7)) //an mou edwses - h an mou edwses kati pou den einai date, den exei exit date
            {
                date tmpD("-");
                exitD.set = false; //date's set == false so it's just a dash!
                //std::cerr << "I'm a dash!: " << exitD.get_date_as_string() << "\n";
            }
            else //to last part einai date ara yay!
            {
                date tmpD(tmp);
                exitD.set_day(tmpD.get_day());
                exitD.set_month(tmpD.get_month());
                exitD.set_year(tmpD.get_year());
                if (isLater(entryD, exitD) == -1) //mpike meta poy bgike
                {
                    //std::cerr << "Error in record dates.\n";
                    exit(-1);
                }
            }
        }
    }
    delete[] cstr;
    delete pch;
    //delete[] parts;
}

record::record(record &r) //copy constructor //kaleitai ws record r1 = r2;
{
    this->id = r.get_id();
    this->fname = r.get_fname();
    this->lname = r.get_lname();
    this->disease = r.get_disease();
    this->country = r.get_country();
    this->entryD = r.get_entryDate();
    this->exitD = r.get_exitDate();
    this->age = r.get_age();
}

record::~record() {}

std::string record::get_id()
{
    return this->id;
}

std::string record::get_fname()
{
    return this->fname;
}

std::string record::get_lname()
{
    return this->lname;
}

std::string record::get_disease()
{
    return this->disease;
}

std::string *record::get_diseasePtr()
{
    return &disease;
}

std::string record::get_country()
{
    return this->country;
}

std::string *record::get_countryPtr()
{
    return &country;
}

date record::get_entryDate()
{
    return this->entryD;
}

date record::get_exitDate()
{
    return this->exitD;
}

date *record::get_entryDatePtr()
{
    return &entryD;
}

date *record::get_exitDatePtr()
{
    return &exitD;
}
int record::get_age()
{
    return age;
}
void record::set_id(std::string id_to_be)
{
    this->id = id_to_be;
    return;
}

void record::set_fname(std::string fn)
{
    this->fname = fn;
    return;
}

void record::set_lname(std::string ln)
{
    this->lname = ln;
    return;
}

void record::set_disease(std::string dis)
{
    this->disease = dis;
    return;
}

void record::set_country(std::string cntr)
{
    this->country = cntr;
    return;
}

void record::set_entryD(std::string enD)
{
    date d(enD);
    entryD = d;
    return;
}

void record::set_exitD(std::string exD)
{
    if (exD == "-")
    {
        //std::cerr << "den exeis exit date\n";
        exitD.set = false;
        return;
    }
    else
    {
        //std::cerr << "exeis exit date\n";
        date d(exD);
        exitD = d;
        //exitD.set = true;
        return;
    }
}
void record::set_age(int a)
{
    age = a;
    return;
}
void record::print_record()
{
    std::cout << id << " " << fname << " " << lname << " " << disease << " " << country << " " << age << " " << entryD.get_date_as_string() << " ";
    if (exitD.set == true)
    {
        std::cout << exitD.get_date_as_string() << "\n";
    }
    else
    {
        std::cout << "\n";
    }

    return;
}
