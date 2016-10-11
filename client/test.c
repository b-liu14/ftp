#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

int main() {
	printf("0 = %d\r\n", _check_port_param("1,1,1,1,1,1"));
	printf("0 = %d\r\n", _check_port_param("255,255,255,255,255,255"));
	printf("0 = %d\r\n", _check_port_param("255,2a5,255,255,255,255"));
	printf("0 = %d\r\n", _check_port_param("255.2a5,255,255,255,255"));
	return 0;
}
