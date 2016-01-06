#pragma once

#include <list>
#include "mySelect.h"
#include "myTCPServer.h"
#include "ITCPRemoteClient.h"
#include "HttpHeaderLexer.h"
#include "HttpHeaderParser.h"
#include "HandshakeClient.h"
#include "WebSocketClient.h"
#include "WebSocketFrame.h"

# define HANDSHAKE_STR "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
# define PING_OPCODE 0x9
# define PONG_OPCODE 0xA
# define CLOSE_OPCODE 0x8
# define BINARY_OPCODE 0x2
# define TEXT_OPCODE 0x1

# define TEXT_FRAME 0
# define BINARY_FRAME 1

class WebSocketServer
{
public:
	WebSocketServer();
	virtual ~WebSocketServer();

	virtual void	start() = 0;
	virtual void	onNewConnectedUser(WebSocketClient *) = 0;
	virtual void	onClientDisconnected(WebSocketClient *) = 0;
	virtual bool	onReceiveFromConnectedClient(WebSocketClient *, char, std::string const &, int) = 0;
	virtual void	update(int, int) = 0;

protected:
	void	startServer(short = 0);
	void	sendPing(WebSocketClient*);
	void	sendBinary(WebSocketClient *, const std::string&, int);
	void	sendText(WebSocketClient *, const std::string&);
	void	disconnectClient(WebSocketClient *);
	void	run();

	bool							_running;
	mySelect						_select;

private:
	myTCPServer						_server;
	std::list<HandshakeClient*>		_waitingHandshake;
	std::list<WebSocketClient*>		_connectedUser;
	HttpHeaderLexer					_lexer;
	HttpHeaderParser				_parser;
	WebSocketFrame					_frame;

	bool			processReceive(int, int);
	bool			processWrite();
	bool			kickGosts(int, int);

	bool			onReceiveNewConnection();

	void			sendPong(WebSocketClient*);
	void			sendClose(WebSocketClient*);

private: //Handshake
	HandshakeClient	*getHandshakeClient(ITCPRemoteClient *);
	bool			proccessHandshake(HandshakeClient *);
	bool			checkHandshake(HttpHeader &);
	bool			replyHandshake(HandshakeClient *, HttpHeader &);

private: //Connected User
	WebSocketClient	*getWebSocketClient(IObservable*);
	bool			receiveFormClient(WebSocketClient *, int, int);
	bool			checkForPong(int, int);

	void			closeWebSocketClient(WebSocketClient *);
	void			receivePing(WebSocketClient *);
	void			receivePong(WebSocketClient *, int, int);
	void			receiveTextMessage(WebSocketClient *, int, int);
	void			receiveBinaryMessage(WebSocketClient *,  int, int);
	void			receiveClose(WebSocketClient *);
};