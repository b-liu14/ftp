#include "prompt.h"
#include "command.h"
// #define DEBUG

/*
 * @brief Convert the string to struct Prompt
 * 		  If failed, return -1
 *		  Otherwise, return 0
 * @param str source stirng to convert from
 * @param pro pointer of destination prompt to convert to
 */
int string2prompt(char* str, Prompt* pro) {
	int len = (int)strlen(str);
	if(str[len-1] == '\n') {
		len--;
		str[len] = '\0';
	}
	// len should not be smaller than 3 because there is a code with three digit.
	// and should not also be bigger than 103 because the max length of param is 100-1;
	if(len < 3 || len > MAX_PROMPT_MESSAGE_LENGTH + 3) {
		printf("500 Length of str(%s) should between 3 and %d\n",
			str, MAX_PROMPT_MESSAGE_LENGTH + 3);
		return -1;
	}
	for(int i = 0; i < 3; i ++) {
		if(! isdigit(str[i])) {
			printf("500 str[%d] = %c should be digit(str = %s)\n", i, str[i], str);
			return -1;
		}
	}
	if(str[3] != ' ' && str[3] != '-' && str[3] != '\0') {
		printf("500 Str[3] = %c should be space, \'-\', or \'\\0\'\n", str[3]);
		return -1;
	}

	pro->code = atoi(str);
	if(len > 3) {
		strncpy(pro->message, str+4, MAX_PROMPT_MESSAGE_LENGTH);
	} else {
		memset(pro->message, 0, MAX_PROMPT_MESSAGE_LENGTH);
	}

	return 0;
}

/*
 * @brief Convert the prompt to string
 * 		  If failed, return -1
 *		  Otherwise, return 0
 * @param pro pointer of source prompt to convert from
 * @param str destination string to convert to
 */
int prompt2string(Prompt* pro, char* str) {
	if(pro->code < 100 || pro->code > 999) {
		printf("500 pro->code(%d) should betwenn 100 and 999\n", pro->code);
		return -1;
	}

	sprintf(str, "%d %s", pro->code, pro->message);

	return 0;
}