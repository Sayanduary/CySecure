#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <mutex>
#include <iostream>

class Logger {
public:
    Logger(const std::string& filename = "logs/cysecure.log");
    void log(const std::string& message);
    
private:
    std::string logFile;
    std::mutex mtx; // Thread-safe logging
};

#endif // LOGGER_HPP


/*std::mutex ensures thread safety when multiple parts of the program log simultaneously.

Constructor lets you specify or default to logs/cysecure.log.*/