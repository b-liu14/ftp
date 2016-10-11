#pragma once

#include "command.h"
#include "utility.h"

typedef struct {
	short code;
	char message[MAX_PROMPT_MESSAGE_LENGTH];
} Prompt;

extern int string2prompt(char* str, Prompt* pro);
extern int prompt2string(Prompt* pro, char* str);

