objects = client.o command.o prompt.o utility.o
flag = -g -Wall -std=c99

client: $(objects)
	cc $(flag) $(objects) -o client

client.o: command.h utility.h prompt.h
	cc $(flag) -c client.c

command.o: command.h utility.h prompt.h
	cc $(flag) -c command.c

prompt.o: command.h utility.h prompt.h
	cc $(flag) -c prompt.c

utility.o: command.h utility.h prompt.h
	cc $(flag) -c utility.c

.PHONY: clean
clean:
	-rm -f client $(objects)