#include "utility.h"

void check_directory(char* directory) {
	int len = (int)strlen(directory);
    if(directory[len - 1] != '/') {
        directory[len] = '/';
        directory[len+1] = '\0';
    }
}