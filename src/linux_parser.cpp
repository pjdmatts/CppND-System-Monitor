#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem()
{
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "PRETTY_NAME")
        {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel()
{
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids()
{
  vector<int> pids;
  DIR *directory = opendir(kProcDirectory.c_str());
  struct dirent *file;
  while ((file = readdir(directory)) != nullptr)
  {
    // Is this a directory?
    if (file->d_type == DT_DIR)
    {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit))
      {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization()
{
  string line;
  string key;
  string value;
  float total = 0;
  float free = 0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "MemTotal:")
        {
          total = stof(value);
        }
        if (key == "MemFree:")
        {
          free = stof(value);
        }
      }
    }
  }
  return (total - free) / total;
}

long LinuxParser::UpTime()
{
  string uptimeString;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptimeString;
  }
  return stol(uptimeString);
}

// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux

long LinuxParser::Jiffies()
{
  long all_jiffies;
  all_jiffies = std::stol(CpuUtilization()[CPUStates::kUser_]) +
                std::stol(CpuUtilization()[CPUStates::kNice_]) +
                std::stol(CpuUtilization()[CPUStates::kSystem_]) +
                std::stol(CpuUtilization()[CPUStates::kIdle_]) +
                std::stol(CpuUtilization()[CPUStates::kIOwait_]) +
                std::stol(CpuUtilization()[CPUStates::kIRQ_]) +
                std::stol(CpuUtilization()[CPUStates::kSoftIRQ_]) +
                std::stol(CpuUtilization()[CPUStates::kSteal_]) +
                std::stol(CpuUtilization()[CPUStates::kGuest_]) +
                std::stol(CpuUtilization()[CPUStates::kGuestNice_]);
  return all_jiffies;
}

long LinuxParser::ActiveJiffies()
{
  long active_jiffies;
  active_jiffies = std::stol(CpuUtilization()[CPUStates::kUser_]) +
                   std::stol(CpuUtilization()[CPUStates::kNice_]) +
                   std::stol(CpuUtilization()[CPUStates::kSystem_]) +
                   std::stol(CpuUtilization()[CPUStates::kIRQ_]) +
                   std::stol(CpuUtilization()[CPUStates::kSoftIRQ_]) +
                   std::stol(CpuUtilization()[CPUStates::kSteal_]);
  return active_jiffies;
}

long LinuxParser::IdleJiffies()
{
  long idle_jiffies;
  idle_jiffies = std::stol(CpuUtilization()[CPUStates::kIdle_]) +
                 std::stol(CpuUtilization()[CPUStates::kIOwait_]);
  return idle_jiffies;
}

vector<string> LinuxParser::CpuUtilization()
{
  string line;
  string cpu;
  string cpu_time;
  vector<string> cpu_time_vector;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while (linestream >> cpu_time)
    {
      cpu_time_vector.push_back(cpu_time);
    }
  }
  return cpu_time_vector;
}

int LinuxParser::TotalProcesses()
{
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "processes")
        {
          return stoi(value);
        }
      }
    }
  }
  return stoi(value);
}

int LinuxParser::RunningProcesses()
{
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "procs_running")
        {
          return stoi(value);
        }
      }
    }
  }
  return stoi(value);
}

// Individual PIDs

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid)
{
  long active_jiffies;
  active_jiffies = std::stol(ProcStats(pid)[ProcStates::pUtime_]) +
                   std::stol(ProcStats(pid)[ProcStates::pStime_]) +
                   std::stol(ProcStats(pid)[ProcStates::pCutime_]) +
                   std::stol(ProcStats(pid)[ProcStates::pCstime_]);
  return active_jiffies;
}

vector<string> LinuxParser::ProcStats(int pid)
{
  string line;
  string data;
  vector<string> proc_stat_vector;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> data)
    {
      proc_stat_vector.push_back(data);
    }
  }
  return proc_stat_vector;
}

string LinuxParser::Command(int pid)
{
  string command;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open())
  {
    std::getline(filestream, command);
  }

  return command;
}

string LinuxParser::Ram(int pid)
{
  string line;
  string key;
  string ramString;
  float ramMB = 0;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> ramString)
      {
        if (key == "VmSize:")
        {
          ramMB = std::stof(ramString) / 1000;
          ramString = std::to_string(ramMB);
        }
      }
    }
  }
  return ramString;
}

string LinuxParser::Uid(int pid)
{
  string line;
  string key;
  string uid;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> uid)
      {
        if (key == "Uid:")
        {
          return uid;
        }
      }
    }
  }
  return uid;
}

string LinuxParser::User(int pid)
{
  string uid;
  string user;
  string line;
  string begin;
  string x;
  string id;
  uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> begin >> x >> id)
      {
        if (id == uid)
        {
          user = begin;
        }
      }
    }
  }
  return user;
}

long LinuxParser::UpTime(int pid)
{
  long starttime;
  long uptime;
  starttime = std::stol(ProcStats(pid)[ProcStates::pStarttime_]);
  uptime = starttime / sysconf(_SC_CLK_TCK);
  return uptime;
}
