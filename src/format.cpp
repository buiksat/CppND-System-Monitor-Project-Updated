#include <string>

#include "format.h"
using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds[[maybe_unused]]) {
  int hour, minute, second;
  hour = int(seconds / 3600);
  minute = int((seconds % 3600) / 60);
  second = (seconds % 3600) % 60;
  return to_string(hour) + ":" + to_string(minute) + ":" + to_string(second);
}
