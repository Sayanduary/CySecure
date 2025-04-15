#include "logger.hpp"
#include "sniffer.hpp"
#include "utils.hpp"
#include <iostream>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <sstream>

Sniffer::Sniffer(const std::string& interface, Logger* loggerPtr)
    : interface_name(interface), handle(nullptr), running(false), total_packets(0), logger(loggerPtr) {

    char errbuf[PCAP_ERRBUF_SIZE];

    handle = pcap_open_live(interface_name.c_str(), BUFSIZ, 1, 1000, errbuf);
    if (handle == nullptr) {
        throw std::runtime_error("Couldn't open device " + interface_name + ": " + errbuf);
    }
}

Sniffer::~Sniffer() {
    if (handle) {
        pcap_close(handle);
    }
}

void Sniffer::start() {
    if (!handle) {
        return;
    }

    running = true;
    pcap_loop(handle, 0, packet_handler, reinterpret_cast<u_char*>(this));
}

void Sniffer::stop() {
    running = false;
    if (handle) {
        pcap_breakloop(handle);
    }
}

void Sniffer::display_stats() {
    std::cout << Color::GREEN << "Total packets captured: " << total_packets << Color::RESET << std::endl;

    if (!protocol_counts.empty()) {
        std::cout << Color::YELLOW << "\nProtocol Distribution:" << Color::RESET << std::endl;
        for (const auto& pair : protocol_counts) {
            std::cout << "  " << pair.first << ": " << pair.second << " packets" << std::endl;
        }
    }

    if (!port_counts.empty()) {
        std::cout << Color::YELLOW << "\nTop Ports:" << Color::RESET << std::endl;
        for (const auto& pair : port_counts) {
            std::cout << "  " << pair.first << ": " << pair.second << " packets" << std::endl;
        }
    }
}

void Sniffer::packet_handler(u_char *user_data, const struct pcap_pkthdr *header, const u_char *packet) {
    Sniffer* sniffer = reinterpret_cast<Sniffer*>(user_data);

    if (!sniffer->running) {
        return;
    }

    sniffer->total_packets++;
    struct ether_header *eth_header = (struct ether_header *)packet;

    if (ntohs(eth_header->ether_type) == ETHERTYPE_IP) {
        struct ip *ip_header = (struct ip*)(packet + sizeof(struct ether_header));

        int src_port = 0;
        int dst_port = 0;
        std::string protocol;

        char src_ip[INET_ADDRSTRLEN];
        char dst_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

        switch(ip_header->ip_p) {
            case IPPROTO_TCP: {
                protocol = "TCP";
                struct tcphdr *tcp_header = (struct tcphdr*)(packet + sizeof(struct ether_header) + ip_header->ip_hl * 4);
                src_port = ntohs(tcp_header->th_sport);
                dst_port = ntohs(tcp_header->th_dport);

                sniffer->port_counts["TCP:" + std::to_string(src_port) + "→" + std::to_string(dst_port)]++;
                break;
            }
            case IPPROTO_UDP: {
                protocol = "UDP";
                struct udphdr *udp_header = (struct udphdr*)(packet + sizeof(struct ether_header) + ip_header->ip_hl * 4);
                src_port = ntohs(udp_header->uh_sport);
                dst_port = ntohs(udp_header->uh_dport);

                sniffer->port_counts["UDP:" + std::to_string(src_port) + "→" + std::to_string(dst_port)]++;
                break;
            }
            case IPPROTO_ICMP:
                protocol = "ICMP";
                break;
            default:
                protocol = "Other";
                break;
        }

        std::stringstream logMsg;
        logMsg << "[Packet " << sniffer->total_packets << "] "
               << "Protocol: " << protocol << ", "
               << "Src IP: " << src_ip << ", Dst IP: " << dst_ip;

        if (protocol == "TCP" || protocol == "UDP") {
            logMsg << ", Src Port: " << src_port << ", Dst Port: " << dst_port;
        }

        std::cout << logMsg.str() << std::endl;

        if (sniffer->logger) {
            sniffer->logger->log(logMsg.str());
        }

        sniffer->protocol_counts[protocol]++;

    } else if (ntohs(eth_header->ether_type) == ETHERTYPE_ARP) {
        sniffer->protocol_counts["ARP"]++;
    } else {
        sniffer->protocol_counts["Other"]++;
    }
}