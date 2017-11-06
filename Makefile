NAME_SERVER:=mailserver
NAME_CLIENT:=mailclient
FLAGS:=-ggdb -g -Wall -Werror -O -std=c++11 -pthread
END_FLAGS:=-lldap -DLDAP_DEPRECATED -llber
CC:=g++

all: ${NAME_SERVER} ${NAME_CLIENT}

${NAME_SERVER}: server/*.cpp server/**/*.cpp
	${CC} ${FLAGS} -o ${NAME_SERVER} server/*.cpp server/**/*.cpp ${END_FLAGS}

${NAME_CLIENT}: client/*.cpp
	${CC} ${FLAGS} -o ${NAME_CLIENT} client/*.cpp

clean:
	rm -f ${NAME_SERVER} ${NAME_CLIENT} *.o
