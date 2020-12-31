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

Process::Process(int pid) {
  pid_ = pid;
  total_cpu_usage_ = LinuxParser::Jiffies();
  proc_times_ = LinuxParser::ActiveJiffies(pid_);
  cpu_util_ = (float)proc_times_ * 100 / total_cpu_usage_;
}
// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
  auto proc_times = LinuxParser::ActiveJiffies(pid_);
  auto total_cpu_usage = LinuxParser::Jiffies();
  // auto cpu_util = proc_times;
  // auto cpu_util = float(proc_times - proc_times_) /
  //                 (float)(total_cpu_usage - total_cpu_usage_);
  auto cpu_util = float(proc_times) / total_cpu_usage;
  // std::cout << proc_times << " " << proc_times_ << " " << total_cpu_usage_
  //          << " " << total_cpu_usage << std::endl;
  // std::cout << proc_times << " " << proc_times_ << std::endl;
  // if (cpu_util > 0) {
  //   std::cout << cpu_util << " "
  //             << "############" << std::endl;
  // }
  // proc_times_ = proc_times;
  // total_cpu_usage_ = total_cpu_usage;
  // cpu_util_ = cpu_util;
  return cpu_util;
}

// TODO: Return the command that generated this process
string Process::Command() {
  // std::cout << LinuxParser::Command(pid_) << std::endl;
  return LinuxParser::Command(pid_);
}

// TODO: Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  // TODO: sort by CPU utilization, need to get it through constructor
  // return a.cpu_util_ > cpu_util_;
  auto a_ram = a.Ram();
  auto b_ram = Ram();
  if (a_ram.size() > b_ram.size()) return false;
  if (a_ram.size() < b_ram.size()) return true;
  return a_ram.compare(b_ram) < 0;
}