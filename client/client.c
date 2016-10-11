#include "command.h"

// file connect mode: -1->unset, 0->PORT, 1->PASV
int mode = -1;
// file socket file descriptor: PORT->listen socketfd, PASV->send socketfd
int listen_socketfd = -1;
// address including ip and port number of server
char server_addr_str[24];

int QUITING = 0;
char directory[MAX_DIR_LENGTH] = "";

int main(int argc, char **argv) {
	int socketfd = -1;
	struct sockaddr_in addr;
	char sentence[MAX_BUFF_LENGTH];

	if ((socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		perror("socket()");
		return 1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
		perror("inet_pton()");
		return 1;
	}

	if (connect(socketfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("connect()");
		return 1;
	}

	receive_or_error(socketfd, sentence);

	while(QUITING == 0) {
        if(fgets(sentence, MAX_BUFF_LENGTH, stdin) == NULL) {
            break;
        }
		handle_message(socketfd, sentence);
	}

	close(socketfd);

	return 0;
}

