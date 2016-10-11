#pragma once

#include "utility.h"

typedef struct {
	char username[MAX_USERNAME_LENGTH];
	char password[MAX_PASSWORD_LENGTH];
} User;


typedef struct {
	User user;
	/* the socket file  descriptor bind with user
	   if have not bind a user, socketfd should be -1*/
	int socketfd;
	/* PASV mode -> listen socket file descriptor  */
	int listen_socketfd;
	/* user state: 0->init, 1->need password, 2->authentic,
	 		       3->PORT mode, 4->PASV mode */
	int state;
	/* a string to store user's address */
	char addr_str[MAX_ADDR_STR_LENGTH];
} UserInfo;

extern UserInfo user_info_list[];

extern int init_user();
extern int check_username(char* username);
extern int check_password(char* password);
extern int new_userinfo(int socketfd);
extern UserInfo* get_userinfo_by_sockedfd(int socketfd);
extern void delete_user_info(UserInfo* ptr_user_info);