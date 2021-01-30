#include "process.h"

#include <unistd.h>

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) { pid_ = pid; }
// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const {
  auto proc_times = LinuxParser::ActiveJiffies(pid_);
  auto total_cpu_usage = LinuxParser::Jiffies();
  auto cpu_util = float(proc_times) / total_cpu_usage;
  //printVariableNameAndValue(proc_times);
  //printVariableNameAndValue(total_cpu_usage);
  return cpu_util;
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  // TODO: sort by CPU utilization, need to get it through constructor
  // return a.cpu_util_ > cpu_util_;
  if (a.CpuUtilization() > this->CpuUtilization()) {
    return true;
  } else {
    if (a.CpuUtilization() < this->CpuUtilization()) {
      return false;
    } else {
      auto a_ram = a.Ram();
      auto b_ram = Ram();
      return a_ram.compare(b_ram) < 0;
    }
  }
}