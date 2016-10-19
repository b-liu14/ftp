#include "utility.h"
#include "user.h"
#include "command.h"

UserInfo user_info_list[MAX_USER_NUM];
int listenfd;		// listen file descriptor
fd_set master;		// master file descriptor list
int fd_max; 		// maximum file descriptor number
const char* ip_str = "127.0.0.1";
char directory[MAX_DIR_LENGTH] = "/tmp";
char sentence[MAX_COMMAND_PARAM_LENGTH];

int parse_argv(int argc, char** argv, int* port, char* directory) {
	for(int i = 1; i < argc; i++) {
		if(strcmp(argv[i], "-port") == 0) {
			if(i >= argc -1) {
				return -1;
			}
			*port = atoi(argv[++i]);
		} else if (strcmp(argv[i], "-root") == 0) {
			if(i >= argc -1) {
				return -1;
			}
			strncpy(directory, argv[++i], MAX_DIR_LENGTH);
		} else {
			return -1;
		}
	}
    check_directory(directory);
	return 0;
}

int main(int argc, char **argv) {
	int port = SERVER_PORT;
	if(parse_argv(argc, argv, &port, directory) == -1) {
		printf("invalid paramter\n");
		return 1;
	}
	printf("port = %d\ndirectory = %s\n", port, directory);
    
	struct sockaddr_in addr;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		perror("socket");
		return 1;
	}
	fcntl(listenfd, F_SETFL, O_NONBLOCK);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("bind");
		return 1;
	}

	if (listen(listenfd, 10) == -1) {
		perror("listen");
		return 1;
	}

	init_user();

	FD_SET(listenfd, &master);
	fd_max = listenfd;

	while (1) {
		fd_set read_fds = master;
		if(select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			return 1;
		}

		for (int i = 0; i <= fd_max; i ++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == listenfd) {
					handle_new_connection();
				} else {
					handle_message(i, sentence);
				}
			}
		}
	}

	close(listenfd);
	return 0;
}
