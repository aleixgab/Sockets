#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <iostream>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_ADDRESS "127.0.0.1"

#define SERVER_PORT 8888

#define PAUSE_AND_EXIT() system("pause"); exit(-1)

void printWSErrorAndExit(const char *msg)
{
	wchar_t *s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	fprintf(stderr, "%s: %S\n", msg, s);
	LocalFree(s);
	PAUSE_AND_EXIT();
}

void client(const char *serverAddrStr, int port)
{
	// TODO-1: Winsock init
	WSADATA wsadata;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iResult != NO_ERROR)
	{
		printWSErrorAndExit("STARTUP ERROR");
	}
	// TODO-2: Create socket (IPv4, datagrams, UDP)

	SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);

	// TODO-3: Create an address object with the server address

	sockaddr_in remoteAddrStr ;
	remoteAddrStr.sin_family = AF_INET;
	remoteAddrStr.sin_port = htons(port);
	inet_pton(AF_INET, SERVER_ADDRESS, &remoteAddrStr.sin_addr);

	while (true)
	{
		// TODO-4:
		// - Send a 'ping' packet to the server
		// - Receive 'pong' packet from the server
		// - Control errors in both cases

		const char* buf_ping = "PING";
		int flag = 0;

		iResult = sendto(s, buf_ping, strlen(buf_ping) + 1, flag, (const struct sockaddr*) & remoteAddrStr, sizeof(remoteAddrStr));
		if (iResult < 1)
		{
			printWSErrorAndExit("ERROR SENDING");
		}

		char* buf_pong = new char[32];
		int sizeofAddr = sizeof(remoteAddrStr);
		iResult = recvfrom(s, buf_pong, 32, flag, (struct sockaddr*) & remoteAddrStr, &sizeofAddr);
		if (iResult < 1)
		{
			printWSErrorAndExit("ERROR RECIVING");
		}
		else
		{
			char* printer = new char[sizeofAddr];//Tamaño del buffer
			memcpy(printer, buf_pong, sizeofAddr);
			std::cout << printer << std::endl;
			delete[] printer;
		}
		delete[] buf_pong;
	}

	// TODO-5: Close socket
	closesocket(s);

	// TODO-6: Winsock shutdown
	iResult = WSACleanup();
	if (iResult != NO_ERROR)
	{
		printWSErrorAndExit("ERROR CLEANING UP");
	}

}

int main(int argc, char **argv)
{
	client(SERVER_ADDRESS, SERVER_PORT);

	PAUSE_AND_EXIT();
}
