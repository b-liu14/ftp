#include "command.h"
#include "prompt.h"

int send_or_error(int socketfd, char* str) {
    strcat(str, "\r\n");
    if(send(socketfd, str, strlen(str), 0) < 0) {
        return -1;
    }
    return 0;
}

int receive_or_error(int socketfd, char* str) {
   
    while(1) {
        int recv_len = (int)recv(socketfd, str, MAX_BUFF_LENGTH, 0);
        str[recv_len] = '\0';
        if (recv_len < 6) {
            return -1;
        } else {
            int printf_len = 0;
            char* p = str;
            while(printf_len < recv_len) {
                printf_len += printf("%s", p);

                // search for the last prompter
                char* last = p;
                for(int i = printf_len - 2; i >= 0; i --) {
                    if(str[i] == '\n') {
                        last = str + i + 1;
                        break;
                    }
                }
		return 0;
                if(last[3] == ' ') {
                   
                    return 0;
                }
                p = str + printf_len;
		printf("while loop end\n");
            }

        }
    }

    return 0;
}

/*
 * @brief Convert the string to command
 * 		  If failed, return -1
 *		  Otherwise, return 0
 * @param str source string to convert from
 * @param command pointer of destination command to convert to
 */
int string2command(char* str, Command* ptr_command) {
    int len = (int)strlen(str);
    if(len > MAX_COMMAND_PARAM_LENGTH + MAX_CMD_LENGTH + 1) {
        return -1;
    }
    if(str[len-1] == '\n') {
        len--;
        str[len] = '\0';
    }

    for (int i = 0; i <= len && i <= MAX_CMD_LENGTH; i ++) {
        if(i == len) {
            strncpy(ptr_command->cmd, str, len+1);
            memset(ptr_command->param, 0, MAX_COMMAND_PARAM_LENGTH);
            return 0;
        } else if(str[i] == ' ') {
            strncpy(ptr_command->cmd, str, i);
            ptr_command->cmd[i] = '\0';
            strncpy(ptr_command->param, str+i+1, MAX_COMMAND_PARAM_LENGTH);
            return 0;
        }
    }


    return -1;
}

/*
 * @brief Deal with input message from socketfd
 * @param socketfd file descriptor of socket which sends the message
 * @param message the message will send to server
 */
void handle_message(int socketfd, char message[]) {
    Command command;
    if(string2command(message, &command) < 0) {
        printf("%s", "500 request violated some internal parsing rule in the server\r\n");
        return;
    }

    handle_command(&command, socketfd, message);
}

/*
 * @brief Deal with varied types of command
 * @param ptr_command pointer of command been send to server
 * @param socketfd file descriptor of socket which sends the command
 */
void handle_command(Command* ptr_command, int socketfd, char* message) {
    if(strcmp(ptr_command->cmd, "PORT") == 0) {
        handle_PORT(socketfd, message);
    } else if(strcmp(ptr_command->cmd, "PASV") == 0) {
        handle_PASV(socketfd, message);
    } else if(strcmp(ptr_command->cmd, "RETR") == 0) {
        handle_RETR(socketfd, message);
    } else if(strcmp(ptr_command->cmd, "STOR") == 0) {
        handle_STOR(socketfd, message);
    } else if(strcmp(ptr_command->cmd, "QUIT") == 0) {
        handle_QUIT(socketfd, message);
    } else if(strcmp(ptr_command->cmd, "ABOR") == 0) {
        handle_ABOR(socketfd, message);
    } else {
        send_or_error(socketfd, message);
        receive_or_error(socketfd, message);
    }
}

/*
 * @brief Deal with command PORT
 * @param ptr_user_info pointer of user_info bind with socketfd
 * @param param parameter of command
 * @param socketfd file descriptor of socket which sends the command
 */
void handle_PASV(int socketfd, char* message) {
    if(listen_socketfd != -1) {
        close(listen_socketfd);
        listen_socketfd = -1;
    }

    send_or_error(socketfd, message);
    receive_or_error(socketfd, message);

    Prompt pro;
    string2prompt(message, &pro);
    if(pro.code == 227) {
        mode = 1;
        const char* left = strchr(message, '(') + 1;
        const char* right = strchr(message, ')');
        strncpy(server_addr_str, left, right - left);
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

void parse_port_str(char* port_str, char* myip, int* port_number) {
    int comma_count = 0;
    int i;
    for(i = 0; i < strlen(port_str); i ++) {
        if(port_str[i] == ',') {
            comma_count ++;
            if(comma_count == 4) {
                break;
            }
            myip[i] = '.';

        } else {
            myip[i] = port_str[i];
        }
    }
    char* tmp = port_str+i+1; // tmp -> "p1.p2\0"
    int p1 = atoi(tmp);
    int p2 = atoi(strchr(tmp, ',')+1);
    *port_number = p1 * 256 + p2;
}

/*
 * @brief Create a new file socket to listen new file connection
 * 			and assign the socket port number to port_str
 * @param ptr_user_info pointer of user_info who send the new file connection
 * @param port_str a string to stores the port number with form "p1.p2"
 * 				   satisfying port number = p1 * 256 + p2
 */
int _create_file_socket(char* port_str) {
    struct sockaddr_in addr;

    if ((listen_socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        return -1;
    }

    char myip[14];
    int port_number;
    parse_port_str(port_str, myip, &port_number);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_number);

    if (inet_pton(AF_INET, myip, &addr.sin_addr) <= 0) {
        return -1;
    }

    if (bind(listen_socketfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        return -1;
    }

    if (listen(listen_socketfd, 1) == -1) {
        return -1;
    }

    return 0;
}

/*
 * @brief Deal with command PORT
 * @param socketfd file descriptor of socket which sends the command
 */
void handle_PORT(int socketfd, char* message) {
    if(listen_socketfd != -1) {
        close(listen_socketfd);
        listen_socketfd = -1;
    }

    char port_str[24];
    Command command;
    string2command(message, &command);
    strcpy(port_str, command.param);
    if(_check_port_param(port_str) == -1) {
        printf("500 Invalid PORT parameter\r\n");
        return;
    }
    if(_create_file_socket(port_str) == -1) {
        printf("400 Error when create file socket\r\n");
        return;
    }

    send_or_error(socketfd, message);
    receive_or_error(socketfd, message);
    Prompt pro;
    string2prompt(message, &pro);
    if(pro.code == 200) {
        mode = 0;
    }
}

// return -1 when failed, otherwise return connect_socketfd
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
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_number);

    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        return -1;
    }

    if (connect(connect_socketfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
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

    // TODO
    // check ../
    char path[MAX_DIR_LENGTH] = "\0";
    strncpy(path, directory, MAX_DIR_LENGTH);
    strncpy(path+strlen(path), filename, MAX_DIR_LENGTH);

    int total = 0;

    FILE* fin;
    if((fin = fopen(filename, "rb")) == NULL) {
        return -1;
    } else {
	return 0;
	}
    return total;
}

// return -1 on failure, 0 on success
int _sendall(int s, char *buff, int *len, char* filename)
{

    char path[MAX_DIR_LENGTH] = "\0";
    strncpy(path, directory, MAX_DIR_LENGTH);
    strncpy(path+strlen(path), filename, MAX_DIR_LENGTH);

    int total = 0;
    FILE* fin;
    if((fin = fopen(filename, "rb")) == NULL) {
        return -1;
    }

    while(! feof(fin)) {
	int n = (int)fread(buff, sizeof(char), MAX_BUFF_LENGTH, fin); 
        total += n;
	send(s, buff, n, 0);
        if(ferror(fin)) {
            fclose(fin);
            return -1;
        }
	
    }

    fclose(fin);

    *len = total; // return number actually sent here
    return 0; // return -1 on failure, 0 on success
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
    char* ptr = buff;
    FILE *fout = fopen(filename, "wb");
    while(1) {
 	n = (int)recv(socketfd, ptr, MAX_BUFF_LENGTH, 0);
        if(n <= 0) {break;}
        total += n;
	fwrite(ptr, sizeof(char), n, fout);	
    }
    fclose(fout);
    if(n == -1) {
        return -1;
    } 
     //   FILE* fout = fopen(filename, "wb");
     //   int left = total;
     //   int n = -1;
     //   while(left > 0) {
     //       left -= (int)fwrite(buff, sizeof(char), MAX_BUFF_LENGTH, fout);
     //       if(ferror(fout)) {
     //           fclose(fout);
     //           return -1;
     //       }
     //   }
        
    return total;
    
}

void handle_RETR(int socketfd, char* message) {

    char filename[100];
    Command command;
    string2command(message, &command);
    strncpy(filename, command.param, 100);
    // strncpy(filename, strchr(message, ' ') + 1, strlen(message));
    // filename[strlen(filename) - 1] = '\0';

    int connectfd = -1;
    if (mode == 1) {

        connectfd = _create_connect_socket(server_addr_str);
        if(connectfd == -1) {
            printf("400 Can not create connect socket\n");
            return;
        }

        send_or_error(socketfd, message);

    } else if (mode == 0) {
        send_or_error(socketfd, message);

        if((connectfd = accept(listen_socketfd, NULL, NULL)) < 0) {
            printf("500 Can not accept on socket %d\n", listen_socketfd);
            return;
        }
    } else {
        printf("500 Please specify PORT/PASV mode before RETR/STOR\n");
        return;
    }

    if(receive_or_error(socketfd, message) == -1) {
        close(connectfd);
        return;
    }

    if(_recv_and_write_all(connectfd, filename) == -1) {
        printf("500 Receive or write error\r\n");
        close(connectfd);
        return;
    }

    receive_or_error(socketfd, message);

    close(connectfd);
}

void handle_STOR(int socketfd, char* message) {
    char filename[100];
    Command command;
    string2command(message, &command);
    strncpy(filename, command.param, 100);

    char buff[MAX_BUFF_LENGTH];
    _read_whole_file(filename, buff);


    int connectfd = -1;
    if(mode == 1) {

        connectfd = _create_connect_socket(server_addr_str);
        if(connectfd == -1) {
            printf("400 Can not create connect socket\n");
            return;
        }

        send_or_error(socketfd, message);

    } else if (mode == 0) {
        send_or_error(socketfd, message);

        if((connectfd = accept(listen_socketfd, NULL, NULL)) < 0) {
            printf("400 Can not accept on socket %d\n", listen_socketfd);
            return;
        }
    } else {
        printf("500 Please specify PORT/PASV mode before RETR/STOR\r\n");
        return;
    }

    if(send_or_error(connectfd, buff) == -1) {
        close(connectfd);
        printf("500 Send error in stor\r\n");
        return;
    } else {
        close(connectfd);
        receive_or_error(socketfd, message);
    }

}

void handle_QUIT(int socketfd, char* message) {
    QUITING = 1;
    send_or_error(socketfd, message);
    receive_or_error(socketfd, message);
}

void handle_ABOR(int socketfd, char* message) {
    handle_QUIT(socketfd, message);

}
