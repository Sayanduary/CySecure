#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

// ANSI color codes for terminal output
namespace Color {
    extern const std::string RESET;
    extern const std::string RED;
    extern const std::string GREEN;
    extern const std::string YELLOW;
    extern const std::string BLUE;
    extern const std::string MAGENTA;
    extern const std::string CYAN;
    extern const std::string WHITE;
    extern const std::string BOLD;
}

// Function declarations
void display_logo();
std::string get_timestamp();
void print_bordered_text(const std::string &text, char border_char = '=', 
                         const std::string &color = Color::BLUE);
std::string select_network_interface();

#endif // UTILS_HPP