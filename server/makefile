objects = server.o command.o user.o utility.o
flag = -g -o

server: $(objects)
	cc $(flag) server -Wall $(objects)

$(objects): utility.h user.h command.h

.PHONY: clean 
clean: 
	-rm -f server $(objects) log.txt
