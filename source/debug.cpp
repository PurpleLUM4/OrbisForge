#include "debug.h"

namespace Debug {
    int sockfd = -1;
    struct sockaddr_in server;
}

bool Debug::open(const char* ip_address, int port) {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        char error_message[1024];
        snprintf(error_message, sizeof(error_message), "DEBUG: Failed to connect to %s:%d", ip_address, port);

        utils_SceKernelNotificationRequest(error_message);

        return false; // Failed
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip_address, &server.sin_addr);
    connect(sockfd, (struct sockaddr*)&server, sizeof(server));

    return true; // Success
}

void Debug::log(const char* text) {
    if (sockfd > 0)
        send(sockfd, text, strlen(text), 0);

    return;
}

void Debug::finish(void) {
    if (sockfd >= 0)
        close(sockfd);

    return;
}

void Debug::crash_handler(int signal) {
    char error[32];

    switch (signal) {
        case 11:
            strcpy(error, "Segmentation Fault");
            break;

        case 4:
            strcpy(error, "Illegal Instruction");
            break;

        case 8:
            strcpy(error, "Floating Point Exception");
            break;

        case 6:
            strcpy(error, "Abort");
            break;

        default:
            strcpy(error, "");
            break;
    }

    char error_message[128];
    snprintf(error_message, sizeof(error_message), "[OrbisForge] Crashed with signal %d (%s)", signal, error);

    log(error_message);
    finish();

    _exit(1);
}