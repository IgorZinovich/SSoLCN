#include<iostream>
#include<stdlib.h>
#include<vector>
#ifdef _WIN32
#include <winsock2.h>
#include <Mstcpip.h>
#pragma		comment (lib,"ws2_32.lib")
typedef SOCKET mySocket;
typedef int acceptInt;
#endif


#define BUFFER_SIZE 1024
#define SIZE_CASH 15
#define STR "ERROR"
#define STR1 "READY"

using namespace std;


mySocket initialSocket(struct sockaddr_in& );
void clear();
void closeMySocket(mySocket);
void recvFile(mySocket, struct sockaddr_in, char*);


void main(void)
{
	struct sockaddr_in addr;
	mySocket s = initialSocket(addr);
	int resp = 0;
	struct sockaddr_in addr1;
	int t = sizeof(addr1);
	
	/*fd_set set;
	set.fd_array[0] = s;
	set.fd_count = 1;
	timeval time;
	time.tv_sec = 5;
	time.tv_usec = 0;
	int p = select(1, &set, NULL, NULL, &time);
	if(FD_ISSET(s, &set)!=0)*/
	closesocket(s);
	WSACleanup();
	system("pause");

}



mySocket initialSocket(struct sockaddr_in& addr)
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


	addr.sin_family = AF_INET;
	addr.sin_port = htons(1024);
	addr.sin_addr.s_addr = 0;

	if (bind(s, (struct sockaddr*) &addr, sizeof(addr)) < 0)
	{
		cout << "error binding" << endl;
		closeMySocket(s);
		return -1;
	}
	return s;
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

void recvFile(mySocket s, sockaddr_in addr, char *path)
{

	FILE *file = fopen(path, "wb");
	
	
	sockaddr_in addr1;
	int t = sizeof(addr1);
	int sizeFile;
	if (recvfrom(s, (char*)sizeFile, sizeof(int), 0, (struct sockaddr*) &addr1, &t) < 0)
	{
		cout << "Error 1"<<endl;
		return;
	}

	if (!file)
	{
		if (sendto(s, STR, sizeof(STR), 0, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		{
			cout << "Error2" << endl;
			return;
		}
	}
	if (sendto(s, STR1, sizeof(STR1), 0, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		cout << "Error3" << endl;
		return;
	}

	vector<char*> cash;

	int sizeDownload = 0;
	char* buf = new char[BUFFER_SIZE];
	for (int i = 0; i < SIZE_CASH; i++)
	{
		if (recvfrom(s, (char*)sizeFile, sizeof(int), 0, (struct sockaddr*) &addr1, &t) < 0)
		{
			cout << "Error 1" << endl;
			return;
		}
	}



}

void clear()
{
#ifdef WIN32
	WSACleanup();
#endif
}