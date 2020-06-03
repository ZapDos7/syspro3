#include "date.h"

using namespace std;

date::date() {
    day = 0;
    month = 0;
    year = 0;
    set = false;
}

date::date(string str)
{
    if (str == "-")
    {
        this->set = false;
        this->day = this->month = this->year = -1;
    }
    else
    {
        char *cstr = new char[str.length() + 1]; //auto 8a kanw tokenize
        strcpy(cstr, str.c_str());               //copy as string to str sto cstr
        char *pch;
        const char delim[2] = "-";
        pch = strtok(cstr, delim);
        short unsigned int counter = 0;
        char *parts[3];
        while (pch != NULL)
        {
            parts[counter] = pch;
            if (counter == 3) //ama dw8ei o,ti na nai gia date, yeet -> this saves us from a seg fault an dw8ei kati san 2-3-4-5
            {
                pch = strtok(NULL, delim);
                break;
            }
            counter++;
            pch = strtok(NULL, delim);
        }
        if (atoi(parts[2]) < 0)
        {
            //cout << "Before Christ.\n";
            exit(-1);
        }
        else
        {
            this->year = atoi(parts[2]);
        }

        if ((atoi(parts[1]) <= 0) || (atoi(parts[1]) > 12))
        {
            //cout << "Wrong month.\n";
            exit(-1);
        }
        else
        {
            this->month = atoi(parts[1]);
        }

        if ((atoi(parts[0]) < 0) || (atoi(parts[0]) > 31))
        {
            //cout << "Wrong day.\n";
            exit(-1);
        }
        else
        {
            this->day = atoi(parts[0]);
        }
        this->set = true;
        delete[] cstr;
    }
}

date::date(date &d) //copy constructor, kaleitai ws: date d1 = d2;
{
    //if (d.get_date_as_string() == "-")
    if (d.set==false)
    {
        this->set = false;
        this->day = this->month = this->year = -1;
    }
    else
    {
        if ((d.month < 0) || (d.month > 12))
        {
            this->print_date();
            d.print_date();
            fprintf(stderr, "Unrecoverable error: Invalid month\n");
            exit(1);
        }
        if ((d.day < 0) || (d.day > 31)) //fixed gia ergasia 2
        {
            this->print_date();
            d.print_date();
            
            fprintf(stderr, "Unrecoverable error: Invalid day\n");
            exit(1);
        }
        /*if ((d.month == 2) && (d.year % 4 == 0) && (d.day > 29))
        { //leap yeared feb
            //fprintf(stderr, "Invalid date format\n");
            exit(1);
        }
        //also fixed gia ergasia 2
        if (((d.month == 4) || (d.month == 6) || (d.month == 9) || (d.month == 11)) && (d.day > 30))
        { //30 day-d months
            //fprintf(stderr, "Invalid date format\n");
            exit(1);
        }*/
        this->day = d.day;
        this->month = d.month;
        this->year = d.year;
        this->set = true;
    }
}

date::~date() {}

int date::get_day() const 
{
    if (this->set == true)
        return this->day;
    else
        return -1;
}

int date::get_month() const 
{
    if (this->set == true)
        return this->month;
    else
        return -1;
}

int date::get_year() const 
{
    if (this->set == true)
        return this->year;
    else
        return -1;
}

std::string date::get_date_as_string()
{
    if (set == true)
    {
        std::string day = std::to_string(this->get_day());
        std::string month = std::to_string(this->get_month());
        std::string year = std::to_string(this->get_year());
        std::string date = day + "-" + month + "-" + year;
        return date;
    }
    else
    {
        return "-";
    }
}

void date::set_day(int d) 
{
    if ((d < 0) || (d > 31))
    {
        //fprintf(stderr, "Invalid day\n");
        exit(1);
    }
    else
    {
        this->day = d;
        this->set = true;
    }
    return;
}

void date::set_month(int m)
{
    if ((m < 0) || (m > 12))
    {
        //fprintf(stderr, "Invalid month\n");
        exit(1);
    }
    else
    {
        this->month = m;
        this->set = true;
    }
    return;
}

void date::set_year(int y)
{
    this->year = y;
    this->set = true;
    return;
}

void date::print_date()
{
    if (this->set == true)
    {
        fprintf(stdout, " %d-%d-%d ", this->get_day(), this->get_month(), this->get_year());
    }
    else
    {
        fprintf(stdout, " - ");
    }
    return;
}

//check if d1 is between d2 & d3 => d2...d1...d3

/*bool*/ short int isLater(const date & d1, const  date & d2) //is d1 later than d2?
{
    if ((d1.set == false) || (d2.set == false))
    {
        //fprintf(stderr, "No dates set to compare!\n");
        return -32; //error
        //allo error analoga to poios einai not set?
    }
    if (d1.get_year() > d2.get_year()) //2009 > 2008
    {
        return -1;
    }
    else if (d1.get_year() == d2.get_year())
    {
        if (d1.get_month() > d2.get_month()) //may after january
        {
            return -1;
        }
        else if (d1.get_month() == d2.get_month())
        {
            if (d1.get_day() > d2.get_day()) //3 > 2
            {
                return -1;
            }
            else //if (d1.get_day()==d2.get_day())
            {
                return 0; //same dates
            }
        }
    }
    //else
    return 1;
}
//isLater(a,b): an a>b -> -1
//an a=b -> 0
//an a<b -> 1
bool isBetween(const date &d, const date& d1, const date& d2) //is d between d1 kai d2
{
    if ((d.set == false) || (d1.set == false) || (d2.set == false))
    {
        //fprintf(stderr, "No dates set to compare!\n");
        exit(-1);
    }
    if ((isLater(d, d1) == -1) && (isLater(d, d2) == 1))
    {
        return true; //pane d1 < d < d2
    }
    else
    {
        return false; //estw kai 1 na pige lathos
    }
    //auti tha xrhsimopoii8ei gia [date1 date2] entoles
}
