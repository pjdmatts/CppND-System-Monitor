#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

// TODO: Return etc etc
vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  processes_.clear();
  for (int pid : pids) {
    processes_.push_back(Process(pid));
  }
  std::sort(processes_.begin(), processes_.end());
  return processes_;
}

std::string System::Kernel() { return string(LinuxParser::Kernel()); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() {
  return string(LinuxParser::OperatingSystem());
}

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }