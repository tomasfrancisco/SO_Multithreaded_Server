FLAGS  = -Wall -g
CC     = gcc
PROG   = simplehttpd
OBJS   = simplehttpd.o

all:	${PROG}

clean:
	rm ${OBJS} ${EXE} *~
  
${PROG}:	${OBJS}
	${CC} ${FLAGS} ${OBJS} -o $@

.c.o:
	${CC} ${FLAGS} $< -c

##########################

simplehttpd.o: simplehttpd.c

