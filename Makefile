SRCS = main.cpp src/ServerConfig.cpp src/Config.cpp src/Location.cpp src/ServerManager.cpp src/ServerSocket.cpp src/ClientSocket.cpp src/Http.cpp src/utils/utils.cpp src/CGI.cpp

OBJS = ${SRCS:.cpp=.o}

NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -g

all: ${NAME}

%.o: %.cpp
	${CXX} ${CXXFLAGS} -c $< -o ${<:.cpp=.o}

${NAME}: ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} -o ${NAME}

clean:
	rm -f ${OBJS}

fclean: clean
	rm -f ${NAME}

re: fclean all

.PHONY: all clean fclean re