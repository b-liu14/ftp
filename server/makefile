objects = server.o command.o user.o utility.o
flag = -g -Wall -std=c99

server: $(objects)
	cc $(flag) $(objects) -o server

server.o: utility.h user.h command.h
	cc $(flag) -c server.c

command.o: utility.h user.h command.h
	cc $(flag) -c command.c

user.o: utility.h user.h command.h
	cc $(flag) -c user.c

utility.o: utility.h user.h command.h
	cc $(flag) -c utility.c

.PHONY: clean
clean:
	-rm -f server $(objects) log.txt
