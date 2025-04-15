#include "utils.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <vector>
#include <pcap.h>

// ANSI color codes definitions
namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string WHITE   = "\033[37m";
    const std::string BOLD    = "\033[1m";
}

// ASCII Art logo
void display_logo() {
    std::cout << Color::CYAN << Color::BOLD;
    std::cout << " ██████╗██╗   ██╗███████╗███████╗ ██████╗██╗   ██╗██████╗ ███████╗" << std::endl;
    std::cout << "██╔════╝╚██╗ ██╔╝██╔════╝██╔════╝██╔════╝██║   ██║██╔══██╗██╔════╝" << std::endl;
    std::cout << "██║      ╚████╔╝ ███████╗█████╗  ██║     ██║   ██║██████╔╝█████╗  " << std::endl;
    std::cout << "██║       ╚██╔╝  ╚════██║██╔══╝  ██║     ██║   ██║██╔══██╗██╔══╝  " << std::endl;
    std::cout << "╚██████╗   ██║   ███████║███████╗╚██████╗╚██████╔╝██║  ██║███████╗" << std::endl;
    std::cout << " ╚═════╝   ╚═╝   ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝" << std::endl;
    std::cout << "         Network Packet Analyzer v1.0 - Your Security Companion" << std::endl;
    std::cout << Color::RESET << std::endl;
}

// Get current timestamp as a formatted string
std::string get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Print text with a border
void print_bordered_text(const std::string &text, char border_char, 
                         const std::string &color) {
    std::string border(text.length() + 4, border_char);
    std::cout << color << border << std::endl;
    std::cout << "| " << text << " |" << std::endl;
    std::cout << border << Color::RESET << std::endl;
}

// Function to list available network interfaces with improved formatting
std::string select_network_interface() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevs;
    std::vector<std::string> devices;
    
    // Get all available devices
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        std::cerr << Color::RED << "Error finding devices: " << errbuf << Color::RESET << std::endl;
        return "";
    }
    
    // Print device list with better formatting
    print_bordered_text("Available Network Interfaces", '-', Color::GREEN);
    int i = 0;
    for (pcap_if_t *d = alldevs; d != nullptr; d = d->next) {
        std::string name = d->name ? d->name : "Unknown";
        std::string description = d->description ? d->description : "No description available";
        devices.push_back(name);
        
        std::cout << Color::CYAN << "[" << i << "] " << Color::BOLD << name << Color::RESET << std::endl;
        std::cout << "    " << description << std::endl;
        i++;
    }
    std::cout << std::endl;
    
    // Get user selection
    int selection = -1;
    if (devices.empty()) {
        std::cout << Color::RED << "No network interfaces found!" << Color::RESET << std::endl;
        pcap_freealldevs(alldevs);
        return "";
    } else if (devices.size() == 1) {
        selection = 0;
        std::cout << Color::GREEN << "Using the only available interface: " 
                  << Color::BOLD << devices[0] << Color::RESET << std::endl;
    } else {
        std::cout << Color::YELLOW << "Enter the number of the interface to use: " << Color::RESET;
        std::cin >> selection;
        while (selection < 0 || selection >= static_cast<int>(devices.size())) {
            std::cout << Color::RED << "Invalid selection. Please enter a number between 0 and "
                    << (devices.size() - 1) << ": " << Color::RESET;
            std::cin >> selection;
        }
    }
    
    std::string selected_device = devices[selection];
    
    // Free the device list
    pcap_freealldevs(alldevs);
    return selected_device;
}