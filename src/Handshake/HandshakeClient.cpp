#include "HandshakeClient.h"

HandshakeClient::HandshakeClient(ITCPRemoteClient *client)
{
	this->_client = client;
	this->_timeout_sec = TIMEOUT_SEC;
}

HandshakeClient::~HandshakeClient()
{
}

ITCPRemoteClient	*HandshakeClient::getSock()
{
	return (this->_client);
}

bool				HandshakeClient::isTimeout(long sec, long usec)
{
	this->_timeout_sec -= sec;
	return (this->_timeout_sec <= 0);
}

bool				HandshakeClient::operator==(ITCPRemoteClient const *other) const
{
	return ((this->_client == other));
}

