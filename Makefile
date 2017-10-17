NAME_SERVER:=mailserver
NAME_CLIENT:=mailclient
FLAGS:=-ggdb -g -Wall -Werror -O -std=c++11
CC:=g++

all: ${NAME_SERVER} ${NAME_CLIENT}

${NAME_SERVER}: server/*.cpp server/**/*.cpp
	${CC} ${FLAGS} -o ${NAME_SERVER} server/*.cpp server/**/*.cpp

${NAME_CLIENT}: client/*.cpp
	${CC} ${FLAGS} -o ${NAME_CLIENT} client/*.cpp

clean:
	rm -f ${NAME_SERVER} ${NAME_CLIENT} *.o
