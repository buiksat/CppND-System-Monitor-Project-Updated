#include <string>

#include "format.h"
using std::string;
using std::to_string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int hour, minute, second;
  string hour_str, minute_str, second_str;
  hour = int(seconds / 3600);
  minute = (seconds % 3600) / 60;
  second = seconds % 60;
  hour_str = (hour < 10) ? "0" + to_string(hour) : to_string(hour);
  minute_str = (minute < 10) ? "0" + to_string(minute) : to_string(minute);
  second_str = (second < 10) ? "0" + to_string(second) : to_string(second);
  return hour_str + ":" + minute_str + ":" + second_str;
}
