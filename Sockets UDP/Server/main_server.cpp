#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <iostream>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

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

void server(int port)
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

	// TODO-3: Force address reuse
	int enable = 1;
	int res = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));
	if (res == SOCKET_ERROR) 
		printWSErrorAndExit("ERROR FORCING REUSE");


	// TODO-4: Bind to a local address
	
	sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);
	bindAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	iResult = bind(s, (const struct sockaddr*) & bindAddr, sizeof(bindAddr));
	iResult = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));
	if (iResult != NO_ERROR)
	{
		printWSErrorAndExit("ERROR SOCKPOT");
	}

	while (true)
	{
		// TODO-5:
		// - Receive 'ping' packet from a remote host
		// - Answer with a 'pong' packet
		// - Control errors in both cases
		int flag = 0;

		char* buf_pong = new char[32];
		int sizeofAddr = sizeof(bindAddr);
		iResult = recvfrom(s, buf_pong, 32, flag, (struct sockaddr*) & bindAddr, &sizeofAddr);
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

		const char* buf_ping = "PONG";

		iResult = sendto(s, buf_ping, strlen(buf_ping) + 1, flag, (const struct sockaddr*)&bindAddr, sizeof(bindAddr));
		if (iResult < 1)
		{
			printWSErrorAndExit("ERROR SENDING");
		}
	}

	// TODO-6: Close socket
	closesocket(s);

	// TODO-7: Winsock shutdown
	iResult = WSACleanup();
	if (iResult != NO_ERROR)
	{
		printWSErrorAndExit("ERROR CLEANING UP");
	}
}

int main(int argc, char **argv)
{
	server(SERVER_PORT);

	PAUSE_AND_EXIT();
}
