#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <unistd.h>
#include <string>

#include "linux_parser.h"
using std::string;
using std::to_string;
using std::vector;
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long UpTime() const;
  bool operator<(Process const& a) const;


 private:
  int pid_;
  float usage_;
  string memory_;
  string command_;
};

#endif