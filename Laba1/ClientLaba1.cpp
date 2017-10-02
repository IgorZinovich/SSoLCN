#include <winsock2.h>
#include <iostream>
#include <stdlib.h>

#define BUFFER 1024 

#pragma comment(lib,"Wsock32.lib")
using namespace std;

SOCKET initial();
void workClient(SOCKET);
char** parcing(char*);
void upload(SOCKET, char*);
void getRes(SOCKET);

int main()
{
	
	SOCKET s = initial();
	workClient(s);
	
	closesocket(s);
	WSACleanup();
	return 0;

}

SOCKET initial()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
	struct sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_port = htons(1280);
	peer.sin_addr.s_addr = inet_addr("127.0.0.1");
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	connect(s, (struct sockaddr*) &peer, sizeof(peer));

	return s;
}

void workClient(SOCKET s)
{
	bool flag = true;
	do
	{
		char cmd[128];
		gets(cmd);
		send(s, cmd, sizeof(cmd), 0);
		char** cmdp = parcing(cmd);
		switch (cmdp[0][0])
		{
		case 'u': upload(s, cmdp[1]); break;
		case 'c': flag = false; break;
		case 't':
		case 'e': getRes(s); break;
		}
	} while (flag);

}

void upload(SOCKET s, char *path)
{
	char* outbuffer = new char[BUFFER + 1];
	byte *b = new byte[BUFFER];

	FILE* file = fopen(path, "rb");

	while (1)
	{
		int size = fread(b, sizeof(byte), BUFFER, file);
		send(s, (char*)b, size, 0);
		if (size != BUFFER) break;
	}
	fclose(file);
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
	} while (str[i] != '\0');
	return response;
}

void getRes(SOCKET s)
{
	char buffer[128];
	recv(s, buffer, sizeof(buffer), 0);
	cout << buffer << endl;
}