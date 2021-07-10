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

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
  return usage_ = static_cast<float>(LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK)) / UpTime();
}

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() const { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long Process::UpTime() const { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const {
  return usage_ > a.usage_;
}
