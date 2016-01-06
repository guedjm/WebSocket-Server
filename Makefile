
NAME		=	libwebsocket_server.a

SRCS		=	src/WebSocketServer.cpp			\
			src/Handshake/HttpHeader.cpp		\
			src/Handshake/HttpHeaderLexer.cpp	\
			src/Handshake/HttpHeaderParser.cpp	\
			src/Handshake/HandshakeClient.cpp	\
			src/Data/WebSocketFrame.cpp		\
			src/Data/WebSocketClient.cpp		\

OBJS		=	$(SRCS:.cpp=.o)

RM		=	rm -rf

CXXFLAGS	=	-I./include -I./src/Handshake -I../basic_abstraction/include -I../cryptopp562/

BUILD_DIR	=	../../build

all:	$(NAME)

$(NAME): $(OBJS)
	ar rc $(NAME) $(OBJS)
	ranlib $(NAME)
	mkdir -p $(BUILD_DIR)
	cp -f $(NAME) $(BUILD_DIR)

clean:
	$(RM) $(OBJS)

fclean:	clean
	$(RM) $(NAME)

re: fclean all
