#include "Triplette.h"
Triplette::Triplette(pid_t p)
{
    pid = p;
    in = out = 0;
}
Triplette::Triplette(){pid = in = out = 0;}
Triplette::~Triplette(){}