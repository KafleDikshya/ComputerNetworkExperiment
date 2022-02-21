
// ScanPorts.cpp : Define the entrance of application program in console.
//

#include "stdafx.h"
#include<iostream>
using namespace std;
#include<WinSock2.h>
#pragma comment (lib,"ws2_32.lib")

#define STATUS_FALIED 0xFFFF          //Error code
unsigned long serverIP;               //Scanned server ip address
long MaxThread = 200;                 //Permissible number of scan threads
long ThreadCount = 0;                 //number of threads which are scanning in progress                                       
long *aa = &ThreadCount;

//Thread for port scanning
DWORD WINAPI ScanPort(LPVOID lpParam)
{
	short Port = *(short*)lpParam;
	InterlockedIncrement(aa);
	
        //Create a stream Socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM,0);
	if (sock == INVALID_SOCKET){
		cout << "Creating socket failed£¡" << endl;
		return 0;

	}
	else{
		//Fill in the address structure of server
		sockaddr_in severAddr;
		severAddr.sin_family = AF_INET;
		severAddr.sin_port = htons(Port);
		severAddr.sin_addr.S_un.S_addr = serverIP;

		//Determine whether the host is on
		connect(sock, (sockaddr*)&severAddr, sizeof(severAddr));

		//Write socket set
                struct fd_set write;
		FD_ZERO(&write);
		FD_SET(sock, &write);

		//Initialize timeout
		struct timeval timeout;
		timeout.tv_sec = 100 / 1000;
		timeout.tv_usec = 0;

		//Setting up timeout
                if (select(0,NULL,&write,NULL,&timeout)>0)
		{
			cout << Port <<",";
		};
		
                //Close socket
                closesocket(sock);


	}

	InterlockedDecrement(aa);
	return 0;
}

void main(int argc, char *argv[])
{
	if (argc != 2){
		cout << "Please input IP of destination host" << endl;
	
	}

	//Bind to socket library
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		cout << "WSAStartup falied!" << GetLastError() << endl;
		ExitProcess(STATUS_FALIED);
	}

	serverIP = inet_addr(argv[1]);
	cout << "The opened ports are£º" << endl;

        //Scan port 1-1024 of sever
	for (int i = 1; i < 1024; i++)
        {
		//Wait if exceeding the maximum permissible number of threads
		while (ThreadCount >= MaxThread)
			Sleep(10);

		//Create threads and scan ports
	        DWORD ThreadID;
	        CreateThread(NULL, 0, ScanPort, (LPVOID)new short(i), 0, &ThreadID);
	}

	//Wait if there are still unfinished threads
	while (ThreadCount>0)
		Sleep(50);

        //Release the bind with Socket library
	WSACleanup();

}