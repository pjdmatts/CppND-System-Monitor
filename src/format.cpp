#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) 
{ 
    long minutes = seconds / 60;
    long hours = minutes / 60;
    return string(std::to_string(hours) + " : " + std::to_string(minutes%60) + " : " + std::to_string(seconds%60)); 
}