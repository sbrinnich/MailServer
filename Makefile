NAME_SERVER:=mailserver
NAME_CLIENT:=mailclient
FLAGS:=-ggdb -g -Wall -Werror -O -std=c++11 -pthread -lldap -DLDAP_DEPRECATED
CC:=g++

all: ${NAME_SERVER} ${NAME_CLIENT}

${NAME_SERVER}: server/*.cpp server/**/*.cpp
	${CC} ${FLAGS} -o ${NAME_SERVER} server/*.cpp server/**/*.cpp /usr/lib/x86_64-linux-gnu/libldap.so /usr/lib/x86_64-linux-gnu/liblber.so

${NAME_CLIENT}: client/*.cpp
	${CC} ${FLAGS} -o ${NAME_CLIENT} client/*.cpp

clean:
	rm -f ${NAME_SERVER} ${NAME_CLIENT} *.o
