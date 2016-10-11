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

// user
#define MAX_USER_NUM 100
#define MAX_ONLINE_USER_NUM 10
#define MAX_USERNAME_LENGTH 32
#define MAX_PASSWORD_LENGTH 100

// buff
#define MAX_BUFF_LENGTH 100000

// prompt
#define MAX_PROMPT_MESSAGE_LENGTH 100

// command
#define MAX_COMMAND_PARAM_LENGTH 100

// add_str
#define MAX_ADDR_STR_LENGTH 24

#define SERVER_PORT 21

#define MAX_DIR_LENGTH 100

extern const char* ip_str;

