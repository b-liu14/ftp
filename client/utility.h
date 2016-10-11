#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>
#include <errno.h>


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>

#define MAX_BUFF_LENGTH 100000
#define MAX_PROMPT_MESSAGE_LENGTH 100
#define MAX_CMD_LENGTH 4
#define SERVER_PORT 21
#define MAX_DIR_LENGTH 100

extern char directory[MAX_DIR_LENGTH];