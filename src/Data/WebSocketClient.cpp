#include "WebSocketClient.h"


WebSocketClient::WebSocketClient(ITCPRemoteClient *socket)
{
	this->_sock = socket;
	this->_waitingPong = false;
	this->_pongInterval = 0;
}

WebSocketClient::~WebSocketClient()
{
}

ITCPRemoteClient		*WebSocketClient::getSock()
{
	return (this->_sock);
}

bool			WebSocketClient::waitingPong() const
{
	return (this->_waitingPong);
}

void			WebSocketClient::pingSended()
{
	this->_waitingPong = true;
	this->_pongInterval = 0;
}

void					WebSocketClient::pongReceived()
{
	this->_waitingPong = false;
}

bool					WebSocketClient::pongTimedOut(long sec, long usec)
{
	this->_pongInterval += (sec * USEC_TO_SEC + sec);
	return (this->_pongInterval > PONG_TIMEOUT_USEC);
}

bool					WebSocketClient::operator==(IObservable const *obs) const
{
	return (this->_sock == obs);
}
