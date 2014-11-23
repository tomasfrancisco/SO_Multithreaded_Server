FLAGS  = -pthread -D_REENTRANT -Wall -g 
CC     = gcc
PROG   = server
OBJS   = config.o server.o

all:	${PROG}

clean:
	rm ${OBJS} ${EXE} *~

${PROG}:	${OBJS}
	${CC} ${FLAGS} ${OBJS} -o $@

.c.o:
	${CC} ${FLAGS} $< -c

##########################

config.o: config.h config.c

server.o: config.h server.c

server: config.o server.o
