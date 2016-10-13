#include "utility.h"
#include "user.h"
#include "command.h"

/*
 * @brief Send one message to socketfd
 * 		  If failed, return -1
 *		  Otherwise, return 0
 * @param socketfd file descritp of socket 
 * @param str string will be send
 */
int send_or_error(int socketfd, char* str) {
	if(send(socketfd, str, strlen(str), 0) < 0) {
		perror("send()");
        return -1;
	}
    return 0;
}

/*
 * @brief Receive message from socket
 * 		  If failed, return -1
 *		  Otherwise, return 0
 * @param socketfd file descritp of socket 
 * @param str string to store the message
 */
int receive_or_error(int socketfd, char* str) {
	if (recv(socketfd, str, MAX_BUFF_LENGTH, 0) < 0) {
		perror("recv()");
        return -1;
	}
    return 0;
}

/*
 * @brief Convert the string to command
 * 		  If failed, return -1
 *		  Otherwise, return 0
 * @param str source string to convert from
 * @param ptr_command pointer of destination command to convert to
 */
int string2command(char* str, Command* ptr_command) {
	int len = (int)strlen(str);
	if(len < 3 || len > MAX_COMMAND_PARAM_LENGTH + 3) {
		printf("500 Length of str(%s) should between 3 and %d\n", 
			str, MAX_COMMAND_PARAM_LENGTH + 3);
		return -1;
	}
	if(str[len-1] == '\n') {
		len--;
		str[len] = '\0';
	}
    if(str[len-1] == '\r') {
        len--;
        str[len] = '\0';
    }

	for (int i = 0; i <= len; i ++) {
		if(i == len) {
			// no param
			if(i > 5) {
				printf("400 str(%s) is a invalid command\n", str);
				return -1;
			}
			strncpy(ptr_command->cmd, str, len);
			memset(ptr_command->param, 0, MAX_COMMAND_PARAM_LENGTH);
			break;
		} else if(str[i] == ' ') {
			strncpy(ptr_command->cmd, str, i);
			ptr_command->cmd[i] = '\0';
			strncpy(ptr_command->param, str+i+1, MAX_COMMAND_PARAM_LENGTH);
			break;
		}
	}

	return 0;
}

/*
 * @brief Deal with input message from socketfd
 * @param socketfd file descriptor of socket which sends the message
 * @param message the message been send to server
 */
void handle_message(int socketfd, char message[]) {
	// handle command from a client
    memset(message, 0, MAX_COMMAND_PARAM_LENGTH);
	int len = (int)recv(socketfd, message, MAX_BUFF_LENGTH, 0);
	if (len <= 0) {
		// got error or connection closed by client
		if (len == 0) {
			// connection closed
			printf("400 Selectserver: socket %d hung up\n", socketfd);
		} else {
			perror("recv");
		}
        UserInfo* ptr_user_info = get_userinfo_by_sockedfd(socketfd);
        delete_user_info(ptr_user_info);
	} else {
		printf("recv on %d: %s\n", socketfd, message);

		Command command;
		if(string2command(message, &command) < 0) {
			send_or_error(socketfd, "500 request violated some internal parsing rule in the server\r\n");
			return;
		}

		handle_command(&command, socketfd);
	}	
}

/*
 * @brief Deal with new connection.
 */
void handle_new_connection() {
	// handle new connections
	int connfd = accept(listenfd, NULL, NULL);
	if (connfd == -1) {
		perror ("accept");
	} else {
		FD_SET(connfd, &master);
		if (connfd > fd_max) {
			fd_max = connfd;
		}
		printf("new connections on socket %d\n", connfd);
		new_userinfo(connfd);
		send_or_error(connfd, "220 ftp server ready.\r\n");
	}
}

/*
 * @brief Deal with varied types of command
 * @param ptr_command pointer of command been send to server
 * @param socketfd file descriptor of socket which sends the command
 */
void handle_command(Command* ptr_command, int socketfd) {
	UserInfo *ptr_user_info = get_userinfo_by_sockedfd(socketfd);
	if(ptr_user_info == NULL) {
		printf("500 Error! socketfd not found in user_info_list!");
	}

	if(strcmp(ptr_command->cmd, "USER") == 0) {
		handle_USER(ptr_user_info, ptr_command->param, socketfd);
	} else if(strcmp(ptr_command->cmd, "PASS") == 0) {
		handle_PASS(ptr_user_info, ptr_command->param, socketfd);
	} else if(strcmp(ptr_command->cmd, "PORT") == 0) {
		handle_PORT(ptr_user_info, ptr_command->param, socketfd);
	} else if(strcmp(ptr_command->cmd, "PASV") == 0) {
		handle_PASV(ptr_user_info, ptr_command->param, socketfd);
	} else if(strcmp(ptr_command->cmd, "SYST") == 0) {
		handle_SYST(ptr_user_info, ptr_command->param, socketfd);
	} else if(strcmp(ptr_command->cmd, "TYPE") == 0) {
		handle_TYPE(ptr_user_info, ptr_command->param, socketfd);
	} else if(strcmp(ptr_command->cmd, "QUIT") == 0) {
		handle_QUIT(ptr_user_info, ptr_command->param, socketfd);
	} else if(strcmp(ptr_command->cmd, "ABOR") == 0) {
		handle_ABOR(ptr_user_info, ptr_command->param, socketfd);
	} else if(strcmp(ptr_command->cmd, "RETR") == 0) {
		handle_RETR(ptr_user_info, ptr_command->param, socketfd);
	} else if(strcmp(ptr_command->cmd, "STOR") == 0) {
		handle_STOR(ptr_user_info, ptr_command->param, socketfd);
	} else {
		send_or_error(socketfd, "500 Syntax error, command unrecognized.\r\n");
	}
}

/*
 * @brief Deal with command USER
 * @param ptr_user_info pointer of user_info bind with socketfd
 * @param param parameter of command
 * @param socketfd file descriptor of socket which sends the command
 */
void handle_USER(UserInfo* ptr_user_info, char* param, int socketfd) {
	if(ptr_user_info->state != 0) {
		send_or_error(socketfd, "502 username has been set\r\n");
	} else {
		if(check_username(param)) {
			ptr_user_info->state = 1;
			send_or_error(socketfd, "331 User name okay, need password.\r\n");
		} else {
			send_or_error(socketfd, "501 User name not found.\r\n");
		}
	}
}

/*
 * @brief Deal with command PASS
 * @param ptr_user_info pointer of user_info bind with socketfd
 * @param param parameter of command
 * @param socketfd file descriptor of socket which sends the command
 */
void handle_PASS(UserInfo* ptr_user_info, char* param, int socketfd) {
	if(ptr_user_info->state == 0) {
		send_or_error(socketfd, "332 Need username for login\r\n");
	} else if(ptr_user_info->state != 1) {
		send_or_error(socketfd, "502 Password has been set\r\n");
	} else {
		if(check_password(param)) {
			ptr_user_info->state = 2;
			send_or_error(socketfd, "230-Welcome to my FTP Archives\r\n");
			send_or_error(socketfd, "230-\r\n");
			send_or_error(socketfd, "230-This site is provided as a public server\r\n");
			send_or_error(socketfd, "230-by Bin Liu(b_liu14@163.com) School of Software\r\n");
			send_or_error(socketfd, "230-Use in violation of any applicable laws is strictly\r\n");
			send_or_error(socketfd, "230-prohibited. We make no guarantees, explicit or implicit, about the\r\n");
			send_or_error(socketfd, "230-contents of this site. Use at your own risk.\r\n");
			send_or_error(socketfd, "230-\r\n");
			send_or_error(socketfd, "230 Guest login ok, access restrictions apply.\r\n");
		} else {
			send_or_error(socketfd, "501 Password is wrong.\r\n");
		}
	}
}

// return 0 when success, -1 when failed
int _check_port_param(char* param) {
	int dc = 0;
	int sc = 0;
	for(int i = 0; i < strlen(param); i ++) {
		if(isdigit(param[i])) {
			dc ++;
		} else if(param[i] == ',') {
			if(dc == 0) {
				return -1;
			} else {
				sc ++;
			}
		} else {
			return -1;
		}
	}
	if(sc != 5) {
		return -1;
	}
	return 0;
}

/*
 * @brief Deal with command PORT
 * @param ptr_user_info pointer of user_info bind with socketfd
 * @param param parameter of command
 * @param socketfd file descriptor of socket which sends the command
 */
void handle_PORT(UserInfo* ptr_user_info, char* param, int socketfd) {
	if(_check_port_param(param)) {
		send_or_error(socketfd, "500 Invalid parameter\r\n");
		return;
	}

	if(ptr_user_info->state == 0) {
		send_or_error(socketfd, "332 Need username for login\r\n");
	} else if(ptr_user_info->state == 1) {
		send_or_error(socketfd, "331 Need password for login\r\n");
	} else {
		if(ptr_user_info->listen_socketfd != -1) {
			close(ptr_user_info->listen_socketfd);
			ptr_user_info->listen_socketfd = -1;
		}
		ptr_user_info->state = 3;
		strncpy(ptr_user_info->addr_str, param, strlen(param));
		send_or_error(socketfd, "200 PORT command successful\r\n");
	}
}


/*
 * @brief Get a random port number between 20000 and 65535
 */
int _get_random_port() {
	return rand() % (65535 - 20000) + 20000;
}

/*
 * @brief Create a new file socket to listen new file connection
 * 			and assign the socket port number to port_str
 *			if failed, return -1, else return 0
 * @param ptr_user_info pointer of user_info who send the new file connection
 * @param port_str a string to stores the port number with form "p1.p2"
 * 				   satisfying port number = p1 * 256 + p2
 */
int _create_file_socket(UserInfo* ptr_user_info, char* port_str) {
	struct sockaddr_in addr;

	if ((ptr_user_info->listen_socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		perror("socket");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	if (inet_pton(AF_INET, ip_str, &addr.sin_addr) <= 0) {
		perror("inet_pton()");
		return -1;
	}

	srand((unsigned)time(NULL));
    int port = _get_random_port();
	addr.sin_port = htons(port);
    printf("port = %d changed = %d\n", port, addr.sin_port);
	while (bind(ptr_user_info->listen_socketfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		if( errno == EADDRINUSE ) {
			addr.sin_port = _get_random_port();
			continue;
		} else {
			perror("bind");
			return -1;
		}
	}

	if (listen(ptr_user_info->listen_socketfd, 1) == -1) {
		perror("listen");
		return -1;
	}

	sprintf(port_str, "%d.%d", port / 256, port % 256);
	return 0;
}

/*
 * @brief Deal with command PASV
 * @param ptr_user_info pointer of user_info bind with socketfd
 * @param param parameter of command
 * @param socketfd file descriptor of socket which sends the command
 */
void handle_PASV(UserInfo* ptr_user_info, char* param, int socketfd) {
    if(strlen(param) > 0) {
        send_or_error(socketfd, "504 PASV should not have parameter\r\n");
    }
    
	if(ptr_user_info->state == 0) {
		send_or_error(socketfd, "332 Need username for login\r\n");
	} else if(ptr_user_info->state == 1) {
		send_or_error(socketfd, "331 Need password for login\r\n");
	} else {
		if(ptr_user_info->listen_socketfd != -1) {
			close(ptr_user_info->listen_socketfd);
			ptr_user_info->listen_socketfd = -1;
		}
		ptr_user_info->state = 4;
		char port_str[8];
		if(_create_file_socket(ptr_user_info, port_str) == -1) {
			send_or_error(socketfd, "425 Failed to enter Passive Mode\r\n");
			return;
		}
		char prompt[100];
		sprintf(prompt, "227 Entering Passive Mode(%s.%s)\r\n", ip_str, port_str);
		// change dot to comma
		for(int i = 0; i < strlen(prompt); i++) {
			if(prompt[i] == '.') {
				prompt[i] = ',';
			}
		}
		send_or_error(socketfd, prompt);
	}
}

void handle_SYST(UserInfo* ptr_user_info, char* param, int socketfd) {
	send_or_error(socketfd, "215 UNIX Type: L8\r\n");
}

void handle_TYPE(UserInfo* ptr_user_info, char* param, int socketfd) {
	if(strcmp(param, "I") == 0) {
		send_or_error(socketfd, "200 Type set to I.\r\n");
	} else {
		send_or_error(socketfd, "504 server supports the verb but does not support the parameter.\r\n");
	}
}

void handle_QUIT(UserInfo* ptr_user_info, char* param, int socketfd) {
    send_or_error(socketfd, "221 Goodbye.\r\n");
    delete_user_info(ptr_user_info);
}

void handle_ABOR(UserInfo* ptr_user_info, char* param, int socketfd) {
	handle_QUIT(ptr_user_info, param, socketfd);
}


/*
 * @brief Create a new connect socket
 * 		  Return -1 when failed, 
 *  	  Otherwise return connect_socketfd		  
 * @param addr_str a string to store address as form "p1,p2,p3,p4,p5,p6"
 *				   p1 ~ p4 is ip address and p5 * 256 + p6 is portnumber
 */
int _create_connect_socket(char* addr_str) {
	char ip[20];
	int port_number;
	int p1, p2, p3, p4, p5, p6;
	sscanf(addr_str, "%d,%d,%d,%d,%d,%d", &p1, &p2, &p3, &p4, &p5, &p6);
	sprintf(ip, "%d.%d.%d.%d", p1, p2, p3, p4);
	port_number = p5 * 256 + p6;

	struct sockaddr_in addr;

	int connect_socketfd;
	if ((connect_socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		perror("socket");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_number);

	if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        return -1;
	}

	if (connect(connect_socketfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return -1;
	}

	return connect_socketfd;
}

/*
 * @brief Read whole file to buff
 * 		  Return -1 when failed, 
 *  	  Otherwise return total size of file
 * @param filename file name(relative path) in current directory or subdirectory.
 * @param buff 
 */
int _read_whole_file(char* filename, char* buff) {
	memset(buff, 0, MAX_BUFF_LENGTH);
	int total = 0;
    int n = 0;

	FILE* fin;
	if((fin = fopen(filename, "rb")) == NULL) {
		return -1;
	}

	while(! feof(fin)) {
		n = (int)fread(buff+total, sizeof(char), MAX_BUFF_LENGTH, fin);
        total += n;
		if(ferror(fin)) {
			fclose(fin);
			return -1;
		}
	}

	fclose(fin);
	return total;
}

/*
 * @brief Send all message to socket
 * 		  Return -1 when failed, 
 *  	  Otherwise return connect_socketfd		  
 * @param socketfd socket file descriptor
 * @param buff string to send
 * @param len max length to send
 */
int _sendall(int socketfd, char *buff, int *len)
{
	int total = 0; // how many bytes we've sent
	int bytesleft = *len; // how many we have left to send
	int n = -1;
	while(total < *len) {
		n = (int)send(socketfd, buff+total, bytesleft, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}
	*len = total; // return number actually sent here
	return n==-1?-1:0; // return -1 on failure, 0 on success
}

/*
 * @brief Receive all message from socket and save it in file.
 * 		  Return -1 when failed, 
 *  	  Otherwise return connect_socketfd		  
 * @param socketfd socket file descriptor
 * @param filename file name(relative path) in current directory or subdirectory.
 */
int _recv_and_write_all(int socketfd, char* filename) {
	int total = 0;
	int n;
	char buff[MAX_BUFF_LENGTH];
	memset(buff, 0, MAX_BUFF_LENGTH);
	char* ptr = buff;
	while(1) {
		n = (int)recv(socketfd, ptr, MAX_BUFF_LENGTH, 0);
		if(n <= 0) {break;}
		total += n;
		ptr += n;
	}
	if(n == -1) {
		return -1;
	} else {
		FILE* fout = fopen(filename, "wb");
		int left = total;
		while(left > 0) {
            int n = (int)fwrite(buff + total - left, sizeof(char), left, fout);
            left -= n;
			if(ferror(fout)) {
				fclose(fout);
				return -1;
			}
		}

		fclose(fout);
		return total;
	}
}

/*
 * @brief Deal with command RETR
 * @param ptr_user_info pointer of user_info bind with socketfd
 * @param param parameter of command
 * @param socketfd file descriptor of socket which sends the command
 */
void handle_RETR(UserInfo* ptr_user_info, char* param, int socketfd) {
	int connectfd;
	char buff[MAX_BUFF_LENGTH];

	if(ptr_user_info->state == 3) {
		connectfd = _create_connect_socket(ptr_user_info->addr_str);
		if(connectfd == -1) {
            send_or_error(ptr_user_info->socketfd, "400 can not create connect socket!\r\n");
			printf("can not create connect socket\r\n");
			return;
		}

	} else if(ptr_user_info->state == 4) {
		if((connectfd = accept(ptr_user_info->listen_socketfd, NULL, NULL)) < 0) {
			printf("can not accept on socket %d\n", ptr_user_info->listen_socketfd);
            send_or_error(ptr_user_info->socketfd, "400 can not accept connect socket!\r\n");
			return;
		}

	} else {
		printf("user on socket %d should specify PORT/PASV mode before RETR/STOR\n", socketfd);
        send_or_error(ptr_user_info->socketfd, "400 user on socket %d should specify PORT/PASV mode before RETR/STOR\r\n");
		return;

	}
    
    char path[MAX_DIR_LENGTH] = "\0";
    strncpy(path, directory, MAX_DIR_LENGTH);
    strncpy(path+strlen(path), param, MAX_DIR_LENGTH);
    int len = _read_whole_file(path, buff);
    if(len == -1) {
        send_or_error(ptr_user_info->socketfd, "550 File failed to be opened!\r\n");
        close(connectfd);
        return;
    }

	char message[100];
	sprintf(message, 
		"150 Opening BINARY mode data connection for %s (%d bytes).\r\n", 
		param, (int)strlen(buff));
	send_or_error(ptr_user_info->socketfd, message);
    
	_sendall(connectfd, buff, &len);

	close(connectfd);

	usleep(100000);

	send_or_error(ptr_user_info->socketfd, "226 Transfer complete\r\n");
}

/*
 * @brief Deal with command STOR
 * @param ptr_user_info pointer of user_info bind with socketfd
 * @param param parameter of command
 * @param socketfd file descriptor of socket which sends the command
 */
void handle_STOR(UserInfo* ptr_user_info, char* param, int socketfd) {
	int connectfd;

	if(ptr_user_info->state == 3) {
		connectfd = _create_connect_socket(ptr_user_info->addr_str);
		if(connectfd == -1) {
			printf("can not create connect socket\n");
			return;
		}

	} else if(ptr_user_info->state == 4) {
		if((connectfd = accept(ptr_user_info->listen_socketfd, NULL, NULL)) < 0) {
			printf("can not accept on socket %d\n", ptr_user_info->listen_socketfd);
			return;
        } else {
            send_or_error(ptr_user_info->socketfd, "150 Connected\r\n");
        }

	} else {
		printf("user on socket %d should specify PORT/PASV mode before RETR/STOR\n", socketfd);
		return;
	}

    
    char path[MAX_DIR_LENGTH] = "\0";
    strncpy(path, directory, MAX_DIR_LENGTH);
    strncpy(path+strlen(path), param, MAX_DIR_LENGTH - strlen(path));
	if(_recv_and_write_all(connectfd, path) == -1) {
		send_or_error(ptr_user_info->socketfd, "550 File failed to be transfered.\r\n");
	} else {
		send_or_error(ptr_user_info->socketfd, "226 Transfer complete\r\n");
	}

	close(connectfd);
}
