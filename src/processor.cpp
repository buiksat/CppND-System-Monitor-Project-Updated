#include "processor.h"
#include "linux_parser.h"
#include <thread>
#include <chrono>


// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  auto prevIdle = LinuxParser::IdleJiffies();
  auto  prevTotal = LinuxParser::Jiffies();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  auto idle = LinuxParser::IdleJiffies();
  auto total = LinuxParser::Jiffies();
  auto diffTotal = static_cast<float>(total - prevTotal);
  auto diffIdle =  static_cast<float>(idle - prevIdle);
  return (diffTotal - diffIdle)/diffTotal;
}