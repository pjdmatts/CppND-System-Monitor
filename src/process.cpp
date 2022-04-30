#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}

int Process::Pid() { return pid_; }

// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
float Process::CpuUtilization() const
{
    long active_ticks = LinuxParser::ActiveJiffies(pid_);
    float total_seconds = LinuxParser::UpTime() - Process::UpTime();
    if(total_seconds != 0.0){
        return ((active_ticks/ sysconf(_SC_CLK_TCK)) / total_seconds);
    } else {
        return 0.0;
    }
    
}

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const &a) const
{
    return a.CpuUtilization() < CpuUtilization();
}