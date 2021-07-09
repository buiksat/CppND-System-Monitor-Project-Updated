#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

Process::Process(int pid) {
    pid_ = pid;
    usage_  = CpuUtilization();
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
  return usage_ = static_cast<float>(LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK)) / UpTime();
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long Process::UpTime() const { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return usage_ > a.usage_;
}
