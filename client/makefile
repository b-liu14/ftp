objects = client.o command.o prompt.o utility.o
flag = -g -o

client: $(objects)
	cc $(flag) client $(objects)

$(objects): command.h utility.h prompt.h

.PHONY: clean 
clean: 
	-rm -f client $(objects)