#include "logger.hpp"
#include <iostream>
#include <mutex>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>

Logger::Logger(const std::string &filename) {
    // Check if the directory exists, if not, create it
    std::string directory = filename.substr(0, filename.find_last_of("/"));
    if (!directory.empty()) {
        struct stat info;
        if (stat(directory.c_str(), &info) != 0) {  // Check if directory exists
            if (mkdir(directory.c_str(), 0777) != 0) {  // Create the directory if it doesn't exist
                std::cerr << "Failed to create directory: " << directory << std::endl;
            } else {
                std::cout << "Created directory: " << directory << std::endl;
            }
        }
    }

    // Open the log file
    log_file.open(filename, std::ios::out | std::ios::app);
    if (!log_file.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    } else {
        std::cout << "Opening log file: " << filename << std::endl;  // Print the path for debugging
    }
}

Logger::~Logger() {
    close();
}

void Logger::log(const std::string &message) {
    std::lock_guard<std::mutex> guard(log_mutex);
    if (log_file.is_open()) {
        std::time_t now = std::time(nullptr);
        char buf[64];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        log_file << "[" << buf << "] " << message << std::endl;
    } else {
        std::cerr << "Log file is not open." << std::endl;
    }
}

void Logger::close() {
    if (log_file.is_open()) {
        log_file.close();
    }
}
