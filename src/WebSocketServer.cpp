#include <iostream>
#include "sha.h"
#include "base64.h"
#include "filters.h"
#include "WebSocketServer.h"

WebSocketServer::WebSocketServer() :
	_running(false)
{
}

WebSocketServer::~WebSocketServer()
{

}

void			WebSocketServer::startServer(short port)
{
	this->_select.addReadFd(&this->_server);
	this->_server.start("", port);
	std::cout << "WebSocket server started on port : " << this->_server.getPort() << std::endl;
	this->run();
}

void			WebSocketServer::sendPing(WebSocketClient *client)
{
	std::string frame;
	this->_frame.initFrame(PING_OPCODE);
	this->_frame.setLenPayload(0);
	this->_frame.setPayloadData("");
	this->_frame.makeFrame(frame);
	client->getSock()->prepareData(frame, frame.length());
	this->_select.addWriteFd(client->getSock());
	client->pingSended();
	std::cout << "Ping sended" << std::endl;
}

void			WebSocketServer::sendPong(WebSocketClient *client)
{
	std::string frame;
	this->_frame.initFrame(PONG_OPCODE);
	this->_frame.setLenPayload(0);
	this->_frame.setPayloadData("");
	this->_frame.makeFrame(frame);
	client->getSock()->prepareData(frame, frame.length());
	this->_select.addWriteFd(client->getSock());
}

void			WebSocketServer::sendClose(WebSocketClient *client)
{
	std::string frame;
	this->_frame.initFrame(CLOSE_OPCODE);
	this->_frame.setLenPayload(0);
	this->_frame.setPayloadData("");
	this->_frame.makeFrame(frame);
	client->getSock()->prepareData(frame, frame.length());
	this->_select.addWriteFd(client->getSock());
}

void			WebSocketServer::sendBinary(WebSocketClient *client, const std::string& binary, int len)
{
	std::string frame;
	this->_frame.initFrame(BINARY_OPCODE);
	this->_frame.setLenPayload(binary.length());
	this->_frame.setPayloadData(binary);
	this->_frame.makeFrame(frame);
	client->getSock()->prepareData(frame, len);
	this->_select.addWriteFd(client->getSock());
}

void			WebSocketServer::sendText(WebSocketClient *client, const std::string& text)
{
	std::string frame;
	this->_frame.initFrame(TEXT_OPCODE);
	this->_frame.setLenPayload(text.length());
	this->_frame.setPayloadData(text);
	this->_frame.makeFrame(frame);
	client->getSock()->prepareData(frame, frame.length());
	this->_select.addWriteFd(client->getSock());
}

void			WebSocketServer::run()
{
	int	r_sec;
	int	r_usec;
	int	elapsed_sec;
	int	elapsed_usec;
	int sec;
	int usec;

	this->_running = true;

	while (this->_running)
	{
		this->_select.start();
		this->_select.getRemainingTime(r_sec, r_usec);
		this->_select.getTimeout(sec, usec);
		elapsed_usec = (r_usec > usec) ? USEC_TO_SEC + usec - r_usec : usec - r_usec;
		elapsed_sec = (r_usec > usec) ? sec - 1 - r_sec : sec - r_sec;
		if (!this->processReceive(elapsed_sec, elapsed_usec) || !this->checkForPong(elapsed_sec, elapsed_usec) 
			|| !this->processWrite() || !this->kickGosts(elapsed_sec, elapsed_usec))
			this->_running = false;
		this->update(elapsed_sec, elapsed_usec);
	}
}

bool			WebSocketServer::processReceive(int sec, int usec)
{
	bool			result;
	HandshakeClient	*hsClient;
	WebSocketClient	*wsClient;

	std::list < IObservable* >  &readyRead = this->_select.getReadyRead();
	for (std::list < IObservable* > ::iterator it = readyRead.begin(); it != readyRead.end(); ++it)
	{
		if (*it == &this->_server)
			result = this->onReceiveNewConnection();
		else if ((wsClient = this->getWebSocketClient(*it)) != NULL)
			result = this->receiveFormClient(wsClient, sec, usec);
		else if ((hsClient = this->getHandshakeClient((ITCPRemoteClient*)*it)) != NULL)
			result = this->proccessHandshake(hsClient);
		else
			result = false;
		if (!result)
			return (result);
	}
	return (true);
}



bool			WebSocketServer::onReceiveNewConnection()
{
	ITCPRemoteClient	*newClient = this->_server.acceptClient();

	if (newClient == NULL)
		return (false);
	this->_waitingHandshake.push_back(new HandshakeClient(newClient));
	this->_select.addReadFd(newClient);
	return (true);
}



HandshakeClient	*WebSocketServer::getHandshakeClient(ITCPRemoteClient *client)
{
	for (std::list<HandshakeClient*>::iterator it = this->_waitingHandshake.begin();
		it != this->_waitingHandshake.end(); ++it)
	{
		if ((**it) == client)
			return (*it);
	}
	return (NULL);
}

bool			WebSocketServer::proccessHandshake(HandshakeClient *client)
{
	bool			valid;
	int				size;
	std::string		buff;

	size = client->getSock()->readData(buff);
	if (size <= 0)
	{
		this->_select.removeReadFd(client->getSock());
		this->_waitingHandshake.remove(client);
		delete client;
		std::cout << "Handshake : Client disconnected" << std::endl;
	}
	else
	{
		std::list < std::pair < HttpHeaderLexer::eHttpTocken, std::string > > &tockens = this->_lexer.lex(buff);
		HttpHeader &header = this->_parser.parse(tockens, valid);
		if (!valid || !this->checkHandshake(header) || !this->replyHandshake(client, header))
		{
			client->getSock()->closeSock();
			this->_select.removeReadFd(client->getSock());
			this->_waitingHandshake.remove(client);
			delete client;
		}
	}
	return (true);
}

bool			WebSocketServer::checkHandshake(HttpHeader &header)
{
  if (header.getMethod() != "GET"
		|| header.getRequestUri() == ""
		|| header.getHttpVersion() != "HTTP/1.1"
		|| !header.headerFieldExist("Host")
		|| !header.headerFieldExist("Upgrade") || header.getHeaderField("Upgrade") != "websocket"
		|| !header.headerFieldExist("Connection")
		|| !header.headerFieldExist("Sec-WebSocket-Key")
		|| !header.headerFieldExist("Sec-WebSocket-Version") || header.getHeaderField("Sec-WebSocket-Version") != "13")
    return (false);
  return (true);
}

bool			WebSocketServer::replyHandshake(HandshakeClient *client, HttpHeader &header)
{
	CryptoPP::SHA		sha;
	std::string			encoded;
	char				res[CryptoPP::SHA::DIGESTSIZE];
	std::stringstream	reply;

	std::string	fullKey;
	fullKey = header.getHeaderField("Sec-WebSocket-Key") + HANDSHAKE_STR;
	sha.CalculateDigest((byte*)res, (const byte *)fullKey.c_str(), fullKey.length());
	CryptoPP::Base64Encoder b(new CryptoPP::StringSink(encoded), false);
	b.Put((const byte *)res, 20);
	b.MessageEnd();
	reply.str("");
	reply << "HTTP/1.1 101 Switching Protocols\r\n";
	reply << "Upgrade: websocket\r\n";
	reply << "Connection: Upgrade\r\n";
	reply << "Sec-WebSocket-Accept: " << encoded << "\r\n\r\n";
	client->getSock()->prepareData(reply.str(), reply.str().size());
	this->_select.addWriteFd(client->getSock());


	WebSocketClient *wsClient = new WebSocketClient(client->getSock());
	this->_waitingHandshake.remove(client);
	this->_connectedUser.push_back(wsClient);
	this->onNewConnectedUser(wsClient);
	delete client;
	return (true);
}

bool			WebSocketServer::receiveFormClient(WebSocketClient *wsClient, int sec, int usec)
{
	int				len;
	std::string		buff;
	unsigned char	opCode;

	len = (wsClient->getSock())->readData(buff);
	if (len <= 0)
	{
		this->_select.removeReadFd(wsClient->getSock());
		this->_connectedUser.remove(wsClient);
		wsClient->getSock()->closeSock();
		delete wsClient->getSock();
		delete wsClient;
	}
	else
	{
		this->_frame.parseFrame(len, buff);
		opCode = this->_frame.getOpcode();
		switch (opCode)
		{
		case CLOSE_OPCODE:
			this->receiveClose(wsClient);
			break;
		case PING_OPCODE:
			this->receivePing(wsClient);
			break;
		case PONG_OPCODE:
			this->receivePong(wsClient, sec, usec);
			break;
		case BINARY_OPCODE:
			this->receiveBinaryMessage(wsClient, sec, usec);
			break;
		case TEXT_OPCODE:
			this->receiveTextMessage(wsClient, sec, usec);
			break;
		}
	}
	return (true);
}

WebSocketClient	*WebSocketServer::getWebSocketClient(IObservable *client)
{
	for (std::list<WebSocketClient*>::iterator it = this->_connectedUser.begin();
		it != this->_connectedUser.end(); ++it)
	{
		if ((**it) == client)
			return (*it);
	}
	return (NULL);
}

bool			WebSocketServer::checkForPong(int sec, int usec)
{
	std::list<WebSocketClient*>::iterator it = this->_connectedUser.begin();

	while (it != this->_connectedUser.end())
	{
		if ((*it)->waitingPong() && (*it)->pongTimedOut(sec, usec))
		{
			this->onClientDisconnected(*it);
			it = this->_connectedUser.erase(it);
			this->closeWebSocketClient(*it);
		}
		else
			++it;
	}
	return (true);
}

bool			WebSocketServer::processWrite()
{
	int					len;
	ITCPRemoteClient	*client;
	HandshakeClient		*handshakeClient;
	WebSocketClient		*connectedClient;

	std::list < IWritable* > &writeReady = this->_select.getReadyWrite();
	for (std::list < IWritable* > ::iterator wrIt = writeReady.begin(); wrIt != writeReady.end(); ++wrIt)
	{
		len = (*wrIt)->writeData();
		if (len <= 0)
		{
			client = static_cast<ITCPRemoteClient*>(*wrIt);
			this->_select.removeWriteFd(client);
			this->_select.removeReadFd(client);
			client->closeSock();

			handshakeClient = this->getHandshakeClient(client);
			if (handshakeClient != NULL)
			{
				this->_waitingHandshake.remove(handshakeClient);
				delete handshakeClient;
			}
			
			connectedClient = this->getWebSocketClient(client);
			if (connectedClient != NULL)
			{
				this->_connectedUser.remove(connectedClient);
				delete connectedClient;
			}
			delete client;
		}
		else if ((*wrIt)->pendingData() == false)
			this->_select.removeWriteFd(*wrIt);
	}
	return (true);
}

bool			WebSocketServer::kickGosts(int sec, int usec)
{
	HandshakeClient	*clientToKick;
	std::list<HandshakeClient*>::iterator it = this->_waitingHandshake.begin();


	while (it != this->_waitingHandshake.end())
	{
		if ((*it)->isTimeout(sec, usec))
		{
			clientToKick = *it;
			it = this->_waitingHandshake.erase(it);
			this->_select.removeReadFd(clientToKick->getSock());
			clientToKick->getSock()->closeSock();
			delete clientToKick->getSock();
			delete clientToKick;
			std::cout << "Handshake : Client kicked for inactivity" << std::endl;
		}
		else
			++it;
	}
	return (true);
}

void			WebSocketServer::closeWebSocketClient(WebSocketClient *client)
{
	this->_select.removeReadFd(client->getSock());
	client->getSock()->closeSock();
	delete client->getSock();
	delete client;
}

void			WebSocketServer::receivePing(WebSocketClient *client)
{
	this->sendPong(client);
}

void			WebSocketServer::receivePong(WebSocketClient *client, int sec, int usec)
{
	client->pongReceived();
}

void			WebSocketServer::receiveBinaryMessage(WebSocketClient *client, int sec, int usec)
{
	std::string msg(this->_frame.getPayloadData().c_str(), this->_frame.getLenPayload());

	this->onReceiveFromConnectedClient(client, BINARY_FRAME, msg, this->_frame.getLenPayload());
}

void			WebSocketServer::receiveTextMessage(WebSocketClient *client, int sec, int usec)
{
	std::string	msg(this->_frame.getPayloadData());

	this->onReceiveFromConnectedClient(client, TEXT_FRAME, msg, msg.length());
}

void			WebSocketServer::receiveClose(WebSocketClient *client)
{
	this->onClientDisconnected(client);
	this->_connectedUser.remove(client);
	this->closeWebSocketClient(client);
}

void			WebSocketServer::disconnectClient(WebSocketClient *client)
{
	this->_connectedUser.remove(client);
	this->closeWebSocketClient(client);
}
