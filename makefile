# Leon e Guilherme

# flags
CFLAGS  = -lm

# object-file
objects = ConexaoRawSocket.o utils.o messages.o commands.o

all: client server

client: client.c $(objects)
	gcc -g client.c -o client $(objects) $(CFLAGS)

server: server.c $(object)
	gcc server.c -o server $(objects) $(CFLAGS)

ConexaoRawSocket.o: ConexaoRawSocket.c ConexaoRawSocket.h
	gcc ConexaoRawSocket.c -c

utils.o: utils.c utils.h
	gcc utils.c -c

messages.o: messages.c messages.h
	gcc messages.c -c

commands.o: commands.c commands.h
	gcc -g commands.c -c $(CFLAGS) 

purge: clean
	-rm -f client server
	
clean:
	-rm -f $(objects)