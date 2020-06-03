#include "Pair.h"

Pair::Pair()
{
    pid = out = in = 0;
}
Pair::Pair(string country) : country(country)
{
    pid = out = in = 0;
}

Pair::~Pair()
{
}
