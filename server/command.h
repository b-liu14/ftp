#pragma once

#include "utility.h"
#include "user.h"

typedef struct {
	char cmd[5];
	char param[MAX_COMMAND_PARAM_LENGTH];
} Command;

extern int listenfd;
extern int fd_max;
extern fd_set master;
extern char directory[MAX_COMMAND_PARAM_LENGTH];

extern int string2command(char* str, Command* ptr_command);

extern void handle_new_connection();
extern void handle_message(int socketfd, char message[]);
extern void handle_command(Command* ptr_command, int socketfd);

extern void handle_USER(UserInfo* ptr_user_info, char* param, int socketfd);
extern void handle_PASS(UserInfo* ptr_user_info, char* param, int socketfd);
extern void handle_PORT(UserInfo* ptr_user_info, char* param, int socketfd);
extern void handle_PASV(UserInfo* ptr_user_info, char* param, int socketfd);
extern void handle_SYST(UserInfo* ptr_user_info, char* param, int socketfd);
extern void handle_TYPE(UserInfo* ptr_user_info, char* param, int socketfd);
extern void handle_QUIT(UserInfo* ptr_user_info, char* param, int socketfd);
extern void handle_ABOR(UserInfo* ptr_user_info, char* param, int socketfd);
extern void handle_RETR(UserInfo* ptr_user_info, char* param, int socketfd);
extern void handle_STOR(UserInfo* ptr_user_info, char* param, int socketfd);