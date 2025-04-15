#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include "utils.hpp"
#include "sniffer.hpp"
#include "logger.hpp"

// Global sniffer pointer for signal handling
Sniffer* global_sniffer = nullptr;

// Signal handler for graceful termination
void signal_handler(int signal) {
    std::cout << std::endl << Color::YELLOW << "Received signal " << signal 
              << ". Gracefully shutting down..." << Color::RESET << std::endl;
    if (global_sniffer) {
        global_sniffer->stop();
    }
}

int main() {
    // Set up signal handling for Ctrl+C
    std::signal(SIGINT, signal_handler);
    
    // Display welcome message and logo
    display_logo();
    std::cout << Color::GREEN << "Started at: " << Color::RESET << get_timestamp() << std::endl;
    std::cout << std::endl;
    
    // Select a network interface
    std::string interface = select_network_interface();
    if (interface.empty()) {
        std::cerr << Color::RED << "No valid interface selected. Exiting." << Color::RESET << std::endl;
        return 1;
    }
    
    print_bordered_text("Starting packet capture on interface: " + interface, '*', Color::MAGENTA);
    
    try {
        Logger logger("logs/packets.log");               // ✅ Initialize Logger
        Sniffer sniffer(interface, &logger);             // ✅ Pass Logger pointer
        global_sniffer = &sniffer;
    
        std::thread capture_thread(&Sniffer::start, &sniffer);
    
        std::cout << Color::YELLOW << "Capturing packets for 10 seconds..." << Color::RESET << std::endl;
    
        for (int i = 0; i <= 20; i++) {
            std::cout << "\r[";
            for (int j = 0; j < 20; j++) {
                if (j < i) std::cout << "█";
                else std::cout << " ";
            }
            std::cout << "] " << (i * 5) << "%" << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        std::cout << std::endl;
    
        std::cout << Color::YELLOW << "Stopping packet capture..." << Color::RESET << std::endl;
        sniffer.stop();
    
        if (capture_thread.joinable()) {
            capture_thread.join();
        }
    
        print_bordered_text("Final Packet Statistics", '=', Color::GREEN);
        sniffer.display_stats();
    
    } catch (const std::exception& e) {
        std::cerr << Color::RED << "Error: " << e.what() << Color::RESET << std::endl;
        return 1;
    }
    
    
    print_bordered_text("CySecure has finished running", '=', Color::CYAN);
    std::cout << Color::GREEN << "Ended at: " << Color::RESET << get_timestamp() << std::endl;
    
    global_sniffer = nullptr;
    return 0;
}