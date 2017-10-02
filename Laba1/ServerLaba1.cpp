#include <winsock2.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER 1024 
#pragma comment(lib,"Wsock32.lib")
using namespace std;

char** parcing(char*);
SOCKET initial();
SOCKET getConnect(SOCKET);
void workServer(SOCKET);
void download(SOCKET, char*);
void upload(SOCKET);
void timeSend(SOCKET);

int main()
{
	SOCKET s = initial();
	while (true){
		SOCKET s2 = getConnect(s);
		workServer(s2);
		closesocket(s2);
	}
	
	WSACleanup();

	return 0;
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
		while (str[i] != ' ' && str[i] != '\0')
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
	} while (str[i-1] != '\0');
	return response;
}

SOCKET initial()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(1280);
	DWORD temp = 0;

	WSAGetLastError();
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	int c = bind(s, (struct sockaddr*)&local, sizeof(local));
	int r = listen(s, 5);
	return s;
}

SOCKET getConnect(SOCKET s)
{
	sockaddr_in remote_addr;
	int size = sizeof(remote_addr);
	SOCKET s2 = accept(s, (struct sockaddr*)&remote_addr, &size);
	return s2;
}

void workServer(SOCKET s)
{
	bool flag = true;
	char lastCmd[128];
	lastCmd[0] = '\0';
	do
	{
		byte inbuffer[BUFFER];
		char command[128];
		int getSize;
		getSize = recv(s, command, sizeof(command), 0);
		
		char** com = parcing(command);
		switch (com[0][0])
		{
		case 'u': download(s, com[1]); break;
		case 'c': flag = false; break;
		case 't': timeSend(s); break;
		case 'e': send(s, lastCmd, sizeof(lastCmd), 0); break;
		default: break;
		}
		strcpy(lastCmd, command);
	} while (flag);
}

void download(SOCKET s, char* path)
{
	int getSize;
	FILE* file = fopen(path, "wb");
	byte* b = new byte[BUFFER];
	while (1)
	{
		getSize = recv(s, (char*)b, BUFFER, 0);
		fwrite(b, sizeof(byte), getSize, file);
		if (getSize != BUFFER) break;
	}
	fclose(file);
}

void timeSend(SOCKET s)
{
	char buffer[10];
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	char* format = "%H:%M:%S";
	strftime(buffer, 10, format, timeinfo);
	cout << "Current Datetime: " << buffer << endl;
	send(s, buffer, sizeof(buffer), 0);
}