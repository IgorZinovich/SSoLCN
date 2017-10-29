#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER 1024 
#define STR "File ended"

#ifdef linux
#include <unistd.h>
#include <netinet/in.h>  //
#include <netinet/tcp.h>
#include <netdb.h>  //
#include <memory.h> //
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
typedef int mySocket;
typedef unsigned int acceptInt;
#endif
#ifdef _WIN32
#include <winsock2.h>
#include <Mstcpip.h>
#pragma		comment (lib,"ws2_32.lib")
typedef SOCKET mySocket;
typedef int acceptInt;
#endif
using namespace std;

char** parcing(char*);
mySocket initial();
mySocket getConnect(mySocket);
void workServer(mySocket);
void download(mySocket, char*);
void upload(mySocket);
void timeSend(mySocket);
void echo(mySocket, char*);
void clear();
void closeMySocket(mySocket);

int main()
{
	mySocket s = initial();
	if (s < 0)
	{
		cout << "Error with socket initialization\npress any button to close application." << endl;
		getchar();
		return 0;
	}

	cout << "server is running" << endl;
	while (true) {
		mySocket s2 = getConnect(s);
		if (s2<0)
		{
			cout << "failed connection attempt" << endl;
			continue;
		}
		workServer(s2);
		closeMySocket(s2);
		cout << "mySocket closed" << endl;
	}
	closeMySocket(s);
	clear();

	return 0;
}

void closeMySocket(mySocket s)
{
#ifdef WIN32
	closesocket(s);
#endif
#ifdef linux
	close(s);
#endif
}

void clear()
{
#ifdef WIN32
	WSACleanup();
#endif
}

char** parcing(char* str)
{
	char** response;
	response = new char*[3];

	int  i = 0;
	int count = 0;
	do
	{
		char* word = new char[BUFFER];
		int j = 0;
		while (str[i] != ' ' && str[i] != '\0' && str[i] != '\n')
		{
			word[j] = str[i];
			i++;
			j++;
		}
		word[j] = '\0';
		word = (char*)realloc(word, j + 1);
		response[count] = word;
		count++;
		i++;
	} while (str[i - 1] != '\0');
	return response;
}

mySocket initial()
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
#endif
	mySocket s = socket(AF_INET, SOCK_STREAM, 0);
	if (s<0)
	{
		return -1;
	}
	struct sockaddr_in local;
#ifdef linux
	memset((char*)&local, '\0', sizeof(local));
#endif
	local.sin_family = AF_INET;
	local.sin_port = htons(1280);

	local.sin_addr.s_addr = htonl(INADDR_ANY);
	int c = bind(s, (struct sockaddr*)&local, sizeof(local));
	if (c < 0)
	{
		closeMySocket(s);
		return -1;
	}
	int r = listen(s, 5);
	if (r < 0)
	{
		closeMySocket(s);
		return -1;
	}

	return s;
}

mySocket getConnect(mySocket s)
{
	sockaddr_in remote_addr;
	acceptInt size = sizeof(remote_addr);
	mySocket s2 = accept(s, (struct sockaddr*)&remote_addr, &size);
	if (s2 < 0)
		return -1;
#ifdef linux
	int keepalive = 1;
	if (setsockopt(s2, SOL_SOCKET, SO_KEEPALIVE, &keepalive, (socklen_t)sizeof(keepalive)) < 0)
	{

		perror("setsockopt(keepalive)");
	}
	int keepcnt = 1;
	if (setsockopt(s2, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, (socklen_t)sizeof(int))<0)
	{

		perror("setsockopt(keepcnt)");
	}
	int keepidle = 30;
	if (setsockopt(s2, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, (socklen_t)sizeof(int))<0)
	{

		perror("setsockopt(keepidle)");
	}
	int keepintvl = 1;
	if (setsockopt(s2, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, (socklen_t)sizeof(int))<0)
	{
		perror("setsockopt(keepalive)");
	}
#endif
#ifdef WIN32
	tcp_keepalive alive;
	alive.onoff = 1;
	alive.keepaliveinterval = 3;
	alive.keepalivetime = 30000;
	DWORD ret;
	DWORD Res = WSAIoctl(s, SIO_KEEPALIVE_VALS, &alive, sizeof(alive), NULL, 0, &ret, NULL, NULL);
	if (Res == SOCKET_ERROR)
	{
		cout << "error with keepalive" << endl;
	}
#endif
	cout << "client " << inet_ntoa(remote_addr.sin_addr) << endl;
	return s2;
}

void workServer(mySocket s)
{
	bool flag = true;
	do
	{
		char command[128];
		int getSize;
		getSize = recv(s, command, sizeof(command), 0);
		if (getSize == -1)
		{
			cout << "The mySocket on the client is closed for an unknown reason" << endl;
			return;
		}
		char** com = parcing(command);
		switch (com[0][0])
		{
		case 'u': download(s, com[1]); break;
		case 'c': flag = false; break;
		case 't': timeSend(s); break;
		case 'e': echo(s, com[1]); break;
		default: break;
		}
	} while (flag);
	cout << "client closed mySocket" << endl;
}

void download(mySocket s, char* path)
{
	int getSize;
	int sizeFile;
	if (recv(s, (char*)(&sizeFile), sizeof(sizeFile), 0)<0)
	{
		cout << "Error retrieving data" << endl;
		return;
	}
	if (sizeFile == -1)
	{
		return;
	}
	FILE* file = fopen(path, "wb");
	cout << "Download file: " << path << endl;
	char* b = new char[BUFFER];
	int sizeDownload = 0;
	while (sizeDownload < sizeFile)
	{

		getSize = recv(s, b, BUFFER, 0);
		if (getSize < 0)
		{
			cout << "Error retrieving data" << endl;
			return;
		}
		sizeDownload += getSize;
		fwrite(b, sizeof(char), getSize, file);
	}
	cout << "Download finish" << endl;
	fclose(file);
}

void timeSend(mySocket s)
{
	char buffer[10];
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	char* format = "%H:%M:%S";
	strftime(buffer, 10, format, timeinfo);
	send(s, buffer, sizeof(buffer), 0);
	cout << "Time sended on client" << endl;
}

void echo(mySocket s, char* str)
{
	if (send(s, str, strlen(str) + 1, 0)<0)
	{
		cout << "Error sending data" << endl;
		return;
	}
}
