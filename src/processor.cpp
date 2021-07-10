#include "processor.h"
#include "linux_parser.h"
#include <thread>
#include <chrono>

double Processor::Utilization() {
  auto prevIdle = LinuxParser::IdleJiffies();
  auto  prevTotal = LinuxParser::Jiffies();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  auto idle = LinuxParser::IdleJiffies();
  auto total = LinuxParser::Jiffies();
  auto diffTotal = static_cast<double >(total - prevTotal);
  auto diffIdle =  static_cast<double>(idle - prevIdle);
  return (diffTotal - diffIdle)/diffTotal;
}