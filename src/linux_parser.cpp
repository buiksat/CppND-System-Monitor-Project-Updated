#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "linux_parser.h"
#include "format.h"


using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          filestream.close();
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    stream.close();
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line;
  string title, number, kB;
  long memTotal, memFree;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> title) {
        if (title == "MemTotal:") {
          linestream >> number >> kB;
          memTotal = stol(number);
        } else if (title == "MemFree:") {
          linestream >> number >> kB;
          memFree = stol(number);
        }
      }
    }
    filestream.close();
  }
  return static_cast<float>(memTotal - memFree)/ memTotal;
}

long LinuxParser::UpTime() {
  string line;
  long upTime{0};
  long idleTime{0};
  std::ifstream file(kProcDirectory + kUptimeFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream ss(line);
    ss >> upTime >> idleTime;
    file.close();
  }
  return upTime;
}

long LinuxParser::Jiffies() {
  auto jiffies = CpuUtilization();
  long output = 0;
  for (auto v : jiffies){
    output += stol(v);
  }
  return output;
}

long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  vector<string> reserve;
  string folder = std::to_string(pid);
  std::ifstream file(kProcDirectory + folder + kStatFilename);
  if (file.is_open()) {
    getline(file, line);
    std::stringstream ss(line);
    while (ss >> value) {
      reserve.emplace_back(value);
    }
  }
  auto utime = stol(reserve[13]);
  auto stime = stol(reserve[14]);
  auto cutime = stol(reserve[15]);
  auto cstime = stol(reserve[16]);
  auto total_time = utime + stime + cstime + cutime;
  file.close();
  return total_time;
}

long LinuxParser::ActiveJiffies() {
  return Jiffies() - IdleJiffies();
}

long LinuxParser::IdleJiffies() {
  auto jiffies = CpuUtilization();
  long idleJiffies = stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
  return idleJiffies;
}

vector<string> LinuxParser::CpuUtilization() {
  string cpu;
  string jiffie;
  string line;
  vector<string> jiffies;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> cpu){
        if (cpu == "cpu") {
          while (linestream >> jiffie){
            jiffies.push_back(jiffie);
          }
          return jiffies;
        }
      }
    }
    filestream.close();
  }
  return {};
}

int LinuxParser::TotalProcesses() {
  string label;
  int number;
  string line;
  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()){
    while(getline(file, line)){
      std::stringstream ss(line);
      ss >> label >> number;
      if (label == "processes"){
        return number;
      }
    }
    file.close();
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  string label;
  int number;
  string line;
  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()){
    while(getline(file, line)){
      std::stringstream ss(line);
      ss >> label >> number;
      if (label == "procs_running"){
        return number;
      }
    }
    file.close();
  }
  return 0;

}
string LinuxParser::Command(int pid) {
  string line;
  line.clear();
  string folder = std::to_string(pid);
  std::ifstream file(kProcDirectory + folder + kCmdlineFilename);
  if (file.is_open()) {
    getline(file, line);
    file.close();
  }
  long unsigned int width = 40;
  if (line.length() > width)
    return line.substr(0, width) + "...";
  return line;
}

string LinuxParser::Ram(int pid) {
  string line, title, memory, kB;
  line.clear();
  string folder = std::to_string(pid);
  std::ifstream file(kProcDirectory + folder + kStatusFilename);
  if (file.is_open()) {
    while(getline(file, line)){
      std::stringstream ss(line);
      ss >> title >> memory >> kB;
      // I have used VmRSS instead of VmSize
      if (title == "VmRSS:"){
        float memoryFloat = stof(memory) / 1024;
        memory = (memoryFloat > 0.001) ? to_string((long)memoryFloat) : "0";
      }
    }
    file.close();
  }
  return memory;
}

string LinuxParser::Uid(int pid) {
  string line, title, number;
  line.clear();
  string folder = std::to_string(pid);
  std::ifstream file(kProcDirectory + folder + kStatusFilename);
  if (file.is_open()) {
    while(getline(file, line)){
      std::stringstream ss(line);
      ss >> title >> number;
      if (title == "Uid:"){
        return number;
      }
    }
    file.close();
  }
  return string();
}

string LinuxParser::User(int pid) {
  string line, userName, str, userID;
  line.clear(); userName.clear();
  string user = Uid(pid);
  std::ifstream file(kPasswordPath);
  if (file.is_open()){
    while(getline(file, line)){
      std::stringstream ss(line);
      getline(ss, userName, ':');
      getline(ss, str, ':');
      getline(ss, userID, ':');
      if (userID == user){
        return userName;
      }
    }
    file.close();
  }
  return string();
}

long LinuxParser::UpTime(int pid) {
  string line, value;
  vector<string> reserve;
  string folder = std::to_string(pid);
  std::ifstream file(kProcDirectory + folder + kStatFilename);
  if (file.is_open()) {
    getline(file, line);
    std::stringstream ss(line);
    while (ss >> value) {
      reserve.emplace_back(value);
    }
    file.close();
  }
  auto startTime = stof(reserve[21]);
  auto Hertz = (float)sysconf(_SC_CLK_TCK);
  auto uptime = (float)LinuxParser::UpTime();
  auto seconds = uptime - (startTime / Hertz) ;
  return static_cast<long>(seconds);
}


