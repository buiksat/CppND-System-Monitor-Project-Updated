#include "process.h"

Process::Process(int pid) {
    pid_ = pid;
    usage_  = CpuUtilization();
    command_ = Command();
    memory_ = Ram();
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
  return usage_ = static_cast<float>(LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK)) / UpTime();
}

string Process::Command() {
  return command_ = LinuxParser::Command(pid_);
}

string Process::Ram()  { return memory_ = LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long Process::UpTime() const { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const {
  return usage_ > a.usage_;
}
