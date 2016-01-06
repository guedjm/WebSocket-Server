#pragma once

#include <time.h>
#include "ITCPRemoteClient.h"

#define TIMEOUT_SEC 10

class HandshakeClient
{
public:
	HandshakeClient(ITCPRemoteClient *);
	~HandshakeClient();

	bool	isTimeout(long, long);
	bool	operator==(ITCPRemoteClient const *) const;

	ITCPRemoteClient	*getSock();

private:
	ITCPRemoteClient	*_client;
	long		_timeout_sec;
};