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

float Process::CpuUtilization() const {
  long total_time;
  float total_time_seconds;
  long utime;
  long stime;
  long cutime;
  long cstime;
  long uptime;
  long starttime;
  float start_time_seconds;
  int pid = pid_;

  utime =
      std::stol(LinuxParser::ProcStats(pid)[LinuxParser::ProcStates::pUtime_]);
  stime =
      std::stol(LinuxParser::ProcStats(pid)[LinuxParser::ProcStates::pStime_]);

  total_time = utime + stime;

  cutime =
      std::stol(LinuxParser::ProcStats(pid)[LinuxParser::ProcStates::pCutime_]);
  cstime =
      std::stol(LinuxParser::ProcStats(pid)[LinuxParser::ProcStates::pCstime_]);

  total_time = total_time + cutime + cstime;

  total_time_seconds = total_time / (float)sysconf(_SC_CLK_TCK);

  uptime = LinuxParser::UpTime();

  starttime = std::stof(
      LinuxParser::ProcStats(pid)[LinuxParser::ProcStates::pStarttime_]);

  start_time_seconds = starttime / (float)sysconf(_SC_CLK_TCK);

  if (float(uptime - start_time_seconds) == 0.0) {
    return 0.0;
  } else {
    return total_time_seconds / float(uptime - start_time_seconds);
  }
}

string Process::Command() { return LinuxParser::Command(Pid()); }

string Process::Ram() { return LinuxParser::Ram(Pid()); }

string Process::User() { return LinuxParser::User(Pid()); }

long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

bool Process::operator<(Process const& a) const {
  return a.CpuUtilization() < CpuUtilization();
}