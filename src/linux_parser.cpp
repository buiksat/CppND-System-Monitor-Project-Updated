#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "linux_parser.h"

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

// TODO: Read and return the system memory utilization
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

// TODO: Read and return the system uptime
long int LinuxParser::UpTime() {
   string line;
  long int upTime, idleTime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
   if (stream.is_open()) {
     std::getline(stream, line);
     std::istringstream linestream(line);
     linestream >> upTime >> idleTime;
     stream.close();
   }
   return upTime;
 }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  auto jiffies = CpuUtilization();
  auto map = LinuxParser::InitialJiffies(jiffies);
  auto allJiffies = std::accumulate(std::begin(map), std::end(map), 0,
                                    [](const long previous, const std::pair<LinuxParser::CPUStates, long>& p)
                                    { return previous + p.second; });
  return allJiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  auto jiffies = CpuUtilization();
  auto map = LinuxParser::InitialJiffies(jiffies);
  auto allJiffies = std::accumulate(std::begin(map), std::end(map), 0,
                                    [](const long previous, const std::pair<LinuxParser::CPUStates, long>& p)
                                    { return previous + p.second; });
  auto idleJiffies = map[LinuxParser::CPUStates::kIdle_] + map[LinuxParser::CPUStates::kIOwait_];
  return allJiffies - idleJiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  auto jiffies = CpuUtilization();
  auto map = LinuxParser::InitialJiffies(jiffies);
  auto idleJiffies = map[LinuxParser::CPUStates::kIdle_] + map[LinuxParser::CPUStates::kIOwait_];
  return idleJiffies;
}

// TODO: Read and return CPU utilization
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
            jiffies.emplace_back(jiffie);
          }
          filestream.close();
          return jiffies;
        }
      }
    }
  }
  filestream.close();
  return {};
}

// TODO: Read and return the total number of processes
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

// TODO: Read and return the number of running processes
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
// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) {
  string line;
  string folder = std::to_string(pid);
  std::ifstream file(kProcDirectory + folder + kCmdlineFilename);
  if (file.is_open()){
    getline(file, line);
  }
  return line; }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) {
  string line, title, memory, kB;
  string folder = std::to_string(pid);
  std::ifstream file(kProcDirectory + folder + kStatusFilename);
  if (file.is_open()) {
    while(getline(file, line)){
      std::stringstream ss(line);
      ss >> title >> memory >> kB;
      if (title == "VmSize:"){
        memory = to_string(stol(memory) / 1024);
      }
    }
    file.close();
  }
  return memory;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) {
  string line, title, num1;
  string folder = std::to_string(pid);
  std::ifstream file(kProcDirectory + folder + kStatusFilename);
  if (file.is_open()) {
    while(getline(file, line)){
      std::stringstream ss(line);
      ss >> title >> num1;
      if (title == "Uid:"){
        return num1;
      }
    }
    file.close();
  }
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) {
  string line, userName, str, userID;
  string user = Uid(pid);
  std::ifstream file(kPasswordPath);
  if (file.is_open()){
    while(getline(file, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::stringstream ss(line);
      ss >> userName >> str >> userID;
      if (userID == user){
        return userName;
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }

std::map<LinuxParser::CPUStates, long> LinuxParser::InitialJiffies(vector<string> const &v){
  std::map<LinuxParser::CPUStates, long> m{
      {LinuxParser::CPUStates::kUser_, stol(v[0])},
      {LinuxParser::CPUStates::kNice_, stol(v[1])},
      {LinuxParser::CPUStates::kSystem_, stol(v[2])},
      {LinuxParser::CPUStates::kIdle_, stol(v[3])},
      {LinuxParser::CPUStates::kIOwait_, stol(v[4])},
      {LinuxParser::CPUStates::kIRQ_, stol(v[5])},
      {LinuxParser::CPUStates::kSoftIRQ_, stol(v[6])},
      {LinuxParser::kSteal_, stol(v[7])},
      {LinuxParser::CPUStates::kGuest_, stol(v[8])},
      {LinuxParser::CPUStates::kGuestNice_, stol(v[9])}
  };
  return m;
}
