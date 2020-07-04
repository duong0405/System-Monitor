#include <dirent.h>
#include <unistd.h>
#include <sstream>
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
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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

float LinuxParser::MemoryUtilization()
{
  string line, key;
  float total = 1, free = 1;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  
  if(stream.is_open())
  {
    while(std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> key;
      
      if(key == "MemTotal:")
      {
        linestream >> total;
      }
      else if(key == "MemFree:")
      {
        linestream >> free;
        break;
      }
    }
  }
  return (total - free) / total;
}

// Read and return system uptime
long LinuxParser::UpTime()
{
  long uptime;
  string line, uptimeStr;
  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if(stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream uptimeStream(line);
    uptimeStream >> uptimeStr;
  }
  uptime = std::stol(uptimeStr);
  return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies()
{
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// Read and return the number of active jiffiles for the system
long LinuxParser::ActiveJiffies()
{
  auto jiffies = CpuUtilization();

  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +
        stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
        stol(jiffies[CPUStates::kSoftIRQ_]) + stol(jiffies[CPUStates::kSteal_]);
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies()
{
  auto jiffies = CpuUtilization();
  return stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization()
{
  string line, cpu, value;
  vector<string> jiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if(stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;

    while(linestream >> value)
    {
      jiffies.push_back(value);
    }
  }
  return jiffies;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
  int processes;
  string key, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if(stream.is_open())
  {
    while(std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> key;
      if(key == "processes")
      {
        linestream >> processes;
        break;
      }
    }
  }
  return processes;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
  int processes;
  string key, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if(stream.is_open())
  {
    while(std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> key;
      if(key == "procs_running")
      {
        linestream >> processes;
        break;
      }
    }
  }
  return processes;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
