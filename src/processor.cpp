#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  auto percent = static_cast<float>( LinuxParser::ActiveJiffies() / LinuxParser::Jiffies());
  return percent;
}