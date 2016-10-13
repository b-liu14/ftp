#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
	printf("tmp = %s\n", tmp);
	int p1 = atoi(tmp);
	int p2 = atoi(strchr(tmp, ',')+1);
	*port_number = p1 * 256 + p2;
}

int main() {
	char str[1000] = "400-wwwww\r\n300 2333";
	int recv_len = strlen(str);
	int printf_len = 0;
	char* p = str;
	while(printf_len < recv_len) {
		printf_len += printf("%s", p) + 1;

		// search for the last prompter
		char* last = p;
		for(int i = printf_len - 2; i >= 0; i --) {
			if(str[i] == '\n') {
				last = str + i + 1;
			}
		}
		
		if(last[3] == ' ') {
			return 0;
		}
		p = str + printf_len;
	}

	return 0;
}
