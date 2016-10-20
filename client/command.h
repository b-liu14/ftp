#pragma once

#include "utility.h"

#define MAX_COMMAND_PARAM_LENGTH 100


typedef struct {
	char cmd[MAX_CMD_LENGTH+1];
	char param[MAX_COMMAND_PARAM_LENGTH];
} Command;

extern int mode;
extern int listen_socketfd;
extern char server_addr_str[24];
extern int QUITING;

extern int receive_or_error(int socketfd, char* str);
extern int send_or_error(int socketfd, char* str);

extern int string2command(char* str, Command* ptr_command);

extern void handle_message(int socketfd, char message[]);
extern void handle_command(Command* ptr_command, int socketfd, char* message);

extern void handle_PORT(int socketfd, char* message);
extern void handle_PASV(int socketfd, char* message);
extern void handle_RETR(int socketfd, char* message);
extern void handle_STOR(int socketfd, char* message);
extern void handle_QUIT(int socketfd, char* message);
extern void handle_ABOR(int socketfd, char* message);