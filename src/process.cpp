#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) 
{
  m_pid = pid;
  m_command = LinuxParser::Command(pid);
  std::string ram = LinuxParser::Ram(pid);
  m_ram = std::stol(ram);
  m_uptime = LinuxParser::UpTime(pid);
  m_user = LinuxParser::User(pid);

  long seconds = LinuxParser::UpTime() - m_uptime;
  long totaltime = LinuxParser::ActiveJiffies(pid);
  try {
    m_utilization = float(totaltime) / float(seconds);

  } catch (...) {
    m_utilization = 0;
  }
}

int Process::Pid() const { return m_pid; }

float Process::CpuUtilization() const { return m_utilization; }

string Process::Command() const { return m_command; }

string Process::Ram() const { return std::to_string(m_ram); }

int Process::getRam() const { return m_ram; }

string Process::User() const { return m_user; }

long int Process::UpTime() const { return m_uptime; }

bool Process::operator<(Process const& a) const
{
  return CpuUtilization() < a.CpuUtilization();
}