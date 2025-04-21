#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <glog/logging.h>
#include <glog/log_severity.h>

class Logger {
 public:
  Logger(const std::string& name, const std::string& log_dir)
      : name_(name), log_dir_(log_dir) {
    google::InitGoogleLogging(name_.c_str());
    google::SetLogDestination(google::INFO, log_dir_.c_str());
    google::SetLogFilenameExtension(".log");
  }

  void Info(const std::string& msg) { LOG(INFO) << msg; }
  void Warning(const std::string& msg) { LOG(WARNING) << msg; }
  void Error(const std::string& msg) { LOG(ERROR) << msg; }

 private:
  std::string name_;
  std::string log_dir_;
};

#endif // LOGGER_HPP