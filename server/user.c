#include "utility.h"
#include "user.h"
#include "command.h"

int init_user() {
	for(int i = 0; i < MAX_ONLINE_USER_NUM; i++) {
		user_info_list[i].socketfd = -1;
	}
	return 0;
}


int check_username(char* username){
	return (strcmp(username, "anonymous") == 0);
}

int check_password(char* password) {
    for(int i = 0; i < strlen(password); i++) {
        if(password[i] == '@') {
            return 1;
        }
    }
    return 0;
}

/*
 * @brief Insert a new user_info in user_info_list
 * 		  If user_info_list has no space, return -1
 *		  Otherwise, return 0
 * @param socketfd socket file descritp of new userinfo
 */
int new_userinfo(int socketfd) {
	int i;
	// find a empty space in usersinfo to insert the userinfo
	for (i = 0; i < MAX_ONLINE_USER_NUM; i++) {
		// find
		if (user_info_list[i].socketfd == -1) {
			user_info_list[i].socketfd = socketfd;
			user_info_list[i].state = 0;
			user_info_list[i].listen_socketfd = -1;
			strcpy(user_info_list[i].working_directory, directory);
            return 0;
		}
	}
	if(i == MAX_ONLINE_USER_NUM) {
		// full, return -1
		return -1;
	}

	return 0;
}

/*
 * @brief Get userinfo by socked file descriptor
 * 		  If found, return the pointer of userinfo
 *		  Otherwise, return NULL
 * @param socketfd socket file descritp of new userinfo
 */
UserInfo* get_userinfo_by_sockedfd(int socketfd) {
	for(int i = 0; i < MAX_ONLINE_USER_NUM; i++) {
		if(user_info_list[i].socketfd == socketfd) {
			return &user_info_list[i];
		}
	}
	return NULL;
}

void delete_user_info(UserInfo* ptr_user_info) {
	if(ptr_user_info->socketfd > 1) {
        FD_CLR(ptr_user_info->socketfd, &master); // remove from master set
		close(ptr_user_info->socketfd);
		ptr_user_info->socketfd = -1;
	}

	if(ptr_user_info->listen_socketfd > 1) {
		close(ptr_user_info->listen_socketfd);
		ptr_user_info->listen_socketfd = -1;
	}
}