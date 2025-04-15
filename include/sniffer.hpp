#ifndef SNIFFER_HPP
#define SNIFFER_HPP

#include <string>
#include <atomic>
#include <pcap.h>
#include <map>
#include "logger.hpp"

class Sniffer {
public:
    Sniffer(const std::string& interface, Logger* loggerPtr); // 🔧 updated
    ~Sniffer();

    void start();
    void stop();
    void display_stats();

private:
    static void packet_handler(u_char *user_data, const struct pcap_pkthdr *header, const u_char *packet);

    std::string interface_name;
    pcap_t *handle;
    std::atomic<bool> running;

    int total_packets;
    std::map<std::string, int> protocol_counts;
    std::map<std::string, int> port_counts;
    Logger* logger;
};

#endif // SNIFFER_HPP
