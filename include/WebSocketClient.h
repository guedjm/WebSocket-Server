#pragma once

#include <ctime>
#include <time.h>
#include "ITCPRemoteClient.h"

#define PONG_TIMEOUT_USEC (10000000000) //10s
#define USEC_TO_SEC 1000000
#define USEC_TO_MS 1000
#define SEC_TO_MS 1000


class WebSocketClient
{
public:
	WebSocketClient(ITCPRemoteClient*);
	~WebSocketClient();

	ITCPRemoteClient	*getSock();
	bool				waitingPong() const;
	void				pingSended();
	void				pongReceived();
	bool				pongTimedOut(long, long);

	bool				operator==(IObservable const *) const;

private:
	bool				_waitingPong;
	ITCPRemoteClient	*_sock;
	long long int		_pongInterval;
};