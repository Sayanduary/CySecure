#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <mutex>

class Logger {
public:
    Logger(const std::string &filename);
    ~Logger();

    void log(const std::string &message);
    void close();

private:
    std::ofstream log_file;
    std::mutex log_mutex; // To ensure thread safety
};

#endif // LOGGER_H