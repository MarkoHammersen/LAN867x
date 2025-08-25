#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>   
#include <time.h>


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <interface>\n", argv[0]);
        return 1;
    }

    const char *ifname = argv[1];

    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock < 0) { perror("socket"); return 1; }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);
    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) { perror("ioctl"); return 1; }
    int ifindex = ifr.ifr_ifindex;

    struct sockaddr_ll addr = {0};
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = ifindex;
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); return 1; }

    printf("Warte auf Raw-Ethernet-Pakete auf %s ...\n", ifname);

    unsigned char buffer[1600];
    long received_bytes = 0;
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);

    while (1) {
        int n = recvfrom(sock, buffer, sizeof(buffer), 0, NULL, NULL);
        if (n <= 0) continue;
        received_bytes += n;

        clock_gettime(CLOCK_MONOTONIC, &now);
        double elapsed = now.tv_sec - start.tv_sec + (now.tv_nsec - start.tv_nsec)/1e9;
        if (elapsed >= 10.0) break; // feste Dauer 10 Sekunden
    }

    clock_gettime(CLOCK_MONOTONIC, &now);
    double elapsed = now.tv_sec - start.tv_sec + (now.tv_nsec - start.tv_nsec)/1e9;
    printf("Empfangen: %ld Bytes\n", received_bytes);
    printf("Dauer: %.2f s\n", elapsed);
    printf("Durchsatz: %.2f Mbit/s\n", received_bytes*8/1e6/elapsed);

    close(sock);
    return 0;
}
