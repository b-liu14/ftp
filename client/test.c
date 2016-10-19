#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <dirent.h>
// #include <sysproto.h>

int main() {
	opendir("/tmp/");
	return 0;	
}
