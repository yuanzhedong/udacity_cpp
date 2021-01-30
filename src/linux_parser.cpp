#include <assert.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;  //?? version
    stream.close();
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // if (std::filesystem:::is_directory(file)) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string memTotal = "MemTotal:";
  string memFree = "MemFree:";
  float Total = findValueByKey<float>(memTotal, kMeminfoFilename);// "/proc/memInfo"
  float Free = findValueByKey<float>(memFree, kMeminfoFilename);
  return (Total - Free) / Total;
}


// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  string line;
  string up_time;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> up_time;
  }
  return up_time.empty()? 0 : std::stol(up_time); // check if it's empty, otherwise random segfault
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  unsigned long long user, nice, system, idle, total_cpu_usage = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    sscanf(line.c_str(), "%*s %llu %llu %llu %llu", &user, &nice, &system,
           &idle);
    total_cpu_usage = user + nice + system + idle;
  }
  return total_cpu_usage;
}

// https://stackoverflow.com/questions/1420426/how-to-calculate-the-cpu-usage-of-a-process-by-pid-in-linux-from-c
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  string line;
  unsigned long int user_time, system_time;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    sscanf(line.c_str(),
           "%*d %*s %*c %*d"  // pid,command,state,ppid

           "%*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu"

           "%lu %lu"  // usertime,systemtime

           "%*ld %*ld %*ld %*ld %*ld %*ld %*llu"

           "%*lu",  // virtual memory size in bytes
           &user_time, &system_time);
  }
  return user_time + system_time;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, key, value;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return std::stoi(value);
        }
      }
    }
  }
  return -1;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, key, value;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }
  return -1;
}

string LinuxParser::Command(int pid) {
  string cmd = std::string(getValueOfFile<std::string>(std::to_string(pid) + kCmdlineFilename));
  return cmd.size() > 50? cmd.substr(50) + "..." : cmd;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line, key, value;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmData:") { // use VmData to get the physical size instead of virtual size(VmSize)
          return value.empty()? "0" : to_string(std::stol(value) / 1024); // or remove the last three chars
        }
      }
    }
  }
  return string("0");
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line, key, value, uid;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          uid = value;
        }
      }
    }
  }
  return uid;
}

string LinuxParser::User(int pid) {
  string target_uid = Uid(pid);

  if (target_uid.empty()) {
    std::cout << pid << std::endl;
  }
  string line;
  string user_name, v1, cur_uid;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user_name >> v1 >> cur_uid) {
        if (cur_uid == target_uid) {
          return user_name;
        }
      }
    }
  }
  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  string line;
  unsigned long long int start_time;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    sscanf(line.c_str(),
           "%*d %*s %*c %*d"  // pid,command,state,ppid

           "%*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu"

           "%*lu %*lu"  // usertime,systemtime

           "%*ld %*ld %*ld %*ld %*ld %*ld %llu"

           "%*lu",  // virtual memory size in bytes
           &start_time);
  }
  /*
  In order to get the unit of time it has been running since start you need to subtract it from the UpTime() of
  the system and so you need to do as follows:
  int upTimePid = UpTime() - stol(var)/sysconf(_SC_CLK_TCK);
  return upTimePid;
  */
  return LinuxParser::UpTime() - start_time / sysconf(_SC_CLK_TCK);
}