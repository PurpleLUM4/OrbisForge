#pragma once

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "utils.h"

namespace Debug {
	extern int sockfd;
	extern struct sockaddr_in server;

	bool open   (const char* ip_address, int port);
	void log    (const char* text);
	void finish (void);

	void crash_handler (int signal);
}