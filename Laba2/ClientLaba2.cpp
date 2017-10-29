#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <Mstcpip.h>
#pragma		comment (lib,"ws2_32.lib")
typedef SOCKET mySocket;
typedef int acceptInt;
#endif

#define BUFFER_SIZE 1024
#define SIZE_CASH 15
#define STR "EROR"

using namespace std;


mySocket initialSocket();
struct sockaddr_in initialAddr(char *ip);
void clear();
void closeMySocket(mySocket);

void sendfile(mySocket, struct sockaddr_in, char*);


int main(void)
{
	
	/*char* ip = "127.0.0.1";

	mySocket s = initialSocket();
	if (s < 0)
	{
		cout << "Error Socket" << endl;
		return 0;
	}
	struct sockaddr_in add = initialAddr(ip);*/
	char* str = "ERROR";
	char* st = strstr(str, STR);
	/*closeMySocket(s);
	clear();*/
	system("pause");
	return 0;

}



mySocket initialSocket()
{
#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) { return -1; }
#endif
	SOCKET s;
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
	{
		return -1;
	}
	return s;
}

struct sockaddr_in initialAddr(char* ip)
{
	sockaddr_in add;
	add.sin_family = AF_INET;
	add.sin_port = htons(1024);
	add.sin_addr.s_addr = inet_addr(ip);
	return add;
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

void sendfile(mySocket s, struct sockaddr_in addr, char* path)
{
	struct sockaddr_in addr1;

	vector<char*> cash;
	char* buf = new char[BUFFER_SIZE];
	FILE *file = fopen(path, "r");
	if (!file)
	{
		cout << "file not open" << endl;
		return;
	}
	fseek(file, 0, SEEK_END);
	int sizeFile = ftell(file);
	fseek(file, 0, SEEK_SET);

	/*if (sendto(s, (char*)&sizeFile, sizeof(sizeFile), 0, (struct sockaddr*) &addr, sizeof(addr)) < 0)
	{
		cout << "can't send size of file" << endl;
		return;
	}*/
	int t = sizeof(addr1);
	if (recvfrom(s, (char*)buf, BUFFER_SIZE, 0, (struct sockaddr*) &addr1, &t) < 0) // а вдруг сервер не сможет создать файл??? 
	{
		fclose(file);
		cout << "not have reponse from server" << endl;
		return;
	}
	if (strstr(buf, STR)) // нет данной подстроки то не ошибка
	{
		fclose(file);
		cout << "Server cant recv file" << endl;
		return;
	}
	int sizeUpload = 0;

	while (sizeUpload < sizeFile)
	{
		int sizeCash = 0;
		for (int i = 0; i < SIZE_CASH; i++)
		{
			sizeCash += fread(buf, sizeof(char), BUFFER_SIZE, file);
			if (sizeUpload == sizeFile)
			{
				break;
			}
			cash.push_back(buf);
		}
		sizeUpload += sizeCash;
		do
		{
			for (int i = 0; i < cash.size(); i++)
			{
				if (sendto(s, cash.at(i), BUFFER_SIZE, 0, (struct sockaddr*) &addr, sizeof(addr)) < 0)
				{
					cout << "can't send size of file" << endl;
					return;
				}
			}
			int resp;

			recvfrom(s, (char*)resp, sizeof(int), 0, (struct sockaddr*) &addr1, &t);

			if (resp == cash.size())
			{
				cash.clear();
				break;
			}
		} while (1);
	}

}