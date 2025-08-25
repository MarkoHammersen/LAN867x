#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>    // <<< hier
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <interface> <dest_mac> <packet_size> <duration_sec>\n", argv[0]);
        return 1;
    }

    const char *ifname = argv[1];
    const char *dest_mac_str = argv[2];
    int packet_size = atoi(argv[3]);
    int duration = atoi(argv[4]);

    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock < 0) { perror("socket"); return 1; }

    // Interface Index ermitteln
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);
    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) { perror("ioctl"); return 1; }
    int ifindex = ifr.ifr_ifindex;

    // Ziel-MAC-Adresse konvertieren
    unsigned char dest_mac[6];
    if (sscanf(dest_mac_str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &dest_mac[0], &dest_mac[1], &dest_mac[2],
               &dest_mac[3], &dest_mac[4], &dest_mac[5]) != 6) {
        fprintf(stderr, "Invalid MAC address\n"); return 1;
    }

    // Ethernet-Frame vorbereiten
    unsigned char *frame = malloc(packet_size);
    if (!frame) { perror("malloc"); return 1; }
    memset(frame, 0xAB, packet_size); // Dummy-Payload
    memcpy(frame, dest_mac, 6);       // Ziel-MAC
    // Absender-MAC holen
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) { perror("ioctl hwaddr"); return 1; }
    memcpy(frame+6, ifr.ifr_hwaddr.sa_data, 6); // Quell-MAC
    frame[12] = 0x08; frame[13] = 0x00; // EtherType = IPv4 (nur Platzhalter)

    struct sockaddr_ll addr = {0};
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = ifindex;
    addr.sll_halen = ETH_ALEN;
    memcpy(addr.sll_addr, dest_mac, 6);

    printf("Sende Raw-Ethernet-Pakete über %s für %d Sekunden...\n", ifname, duration);

    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    long sent = 0;
    while (1) {
        if (sendto(sock, frame, packet_size, 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("sendto"); break;
        }
        sent++;
        clock_gettime(CLOCK_MONOTONIC, &now);
        double elapsed = now.tv_sec - start.tv_sec + (now.tv_nsec - start.tv_nsec)/1e9;
        if (elapsed >= duration) break;
    }

    clock_gettime(CLOCK_MONOTONIC, &now);
    double elapsed = now.tv_sec - start.tv_sec + (now.tv_nsec - start.tv_nsec)/1e9;
    printf("Gesendet: %ld Pakete à %d Bytes\n", sent, packet_size);
    printf("Dauer: %.2f s\n", elapsed);
    printf("Durchsatz: %.2f Mbit/s\n", sent * packet_size * 8 / 1e6 / elapsed);

    free(frame);
    close(sock);
    return 0;
}
