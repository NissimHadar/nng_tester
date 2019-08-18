#include <WinSock2.h>
#include <memory>

#include <iostream>
#include <chrono>
#include <ctime>    

#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>

#include "nanomsg_socket.hpp"

#pragma warning ( disable : 4996) // This is to disable the ctime 'unsafe' warning.

void fatal(const char *func, int rv)
{
	std::cerr << func << ":" << nng_strerror(rv) << std::endl;
}

int server(const char *url)
{
	std::unique_ptr<NanomsgSocket> sock;
	sock = std::make_unique<NanomsgSocket>(NanomsgSocket::NNG_PUB);

	int rv;
	if ((rv = nng_listen(*sock, url, NULL, NNG_FLAG_NONBLOCK)) < 0) {
		fatal("nng_listen", rv);
	}

	for (;;) 
	{
		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char *d = std::ctime(&now);

		std::cout << "SERVER: PUBLISHING DATE " << d;
		if ((rv = nng_send(*sock, d, strlen(d) + 1, 0)) != 0) 
		{
			fatal("nng_send", rv);
		}
		Sleep(1000);
	}
}

int client(const char *url, const char *name)
{
	std::unique_ptr<NanomsgSocket> sock;
	sock = std::make_unique<NanomsgSocket>(NanomsgSocket::NNG_SUB);

	// subscribe to everything (empty means all topics)
	int rv;
	if ((rv = nng_setopt(*sock, NNG_OPT_SUB_SUBSCRIBE, "", 0)) != 0) 
	{
		fatal("nng_setopt", rv);
	}
	
	if ((rv = nng_dial(*sock, url, NULL, NNG_FLAG_NONBLOCK)) != 0) 
	{
		fatal("nng_dial", rv);
	}

	struct pollfd pfd;

	int fd;
	nng_getopt_int(*sock, NNG_OPT_RECVFD, &fd);
	pfd.fd = fd;

	pfd.events = POLLIN;
	pfd.revents = 0;

	for (;;) 
	{
		int state = WSAPoll(&pfd, 1, 0);

		if (state == 0) 
		{
			std::cout << "waiting" << std::endl;
			Sleep(200);
		}
		else if (state == 1)
		{
			char *buf = NULL;
			size_t sz;
			if ((rv = nng_recv(*sock, &buf, &sz, NNG_FLAG_ALLOC)) == 0) 
			{
				std::cout << "CLIENT (" << name << "): RECEIVED " << buf;
			}
			else 
			{
				fatal("nng_recv", rv);
			}
		}
		else
		{
			fatal("WSAPoll", state);
		}
	}
}

int main(const int argc, const char *argv[])
{
	if ((argc >= 2) && (strcmp("server", argv[1]) == 0))
	{
		return (server(argv[2]));
	}

	if ((argc >= 3) && (strcmp("client", argv[1]) == 0))
	{
		return (client (argv[2], argv[3]));
	}

	std::cerr << "Usage: pubsub server|client <URL> <ARG> ..." << std::endl;
	
	return 1;
}