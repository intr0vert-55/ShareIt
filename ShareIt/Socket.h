#pragma once
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
class Socket {
	protected:
		WSAData wsaData;
		SOCKET tcpSocket;
		int checkForError(SOCKET* socket1, SOCKET* socket2, int result, std::string message);
		int checkForError(SOCKET* socket1, SOCKET* socket2, std::string message);
		int checkForError(SOCKET* socket1, int result, std::string message);
		int checkForError(SOCKET* socket1, std::string message);
		int checkForError(int data, std::string message);
		int prepareSocket();
		void closeAndCleanup(SOCKET* socket1, SOCKET* socket2);
		void closeAndCleanup(SOCKET* socket1);
		virtual std::string getFilePath() = 0;
		bool sendCharPtr(SOCKET* socket, const char* buffer, int bufferSize, std::string errorMessage);
		bool sendCharPtr(SOCKET* socket1, SOCKET* socket2, const char* buffer, int bufferSize, std::string errorMessage);
		bool recvCharPtr(SOCKET* socket, char* buffer, int bufferSize, std::string errorMessage);
		bool recvCharPtr(SOCKET* socket1, SOCKET* socket2, char* buffer, int bufferSize, std::string errorMessage);
	public :
		virtual int setup() = 0;		// Facade Pattern
	private :
		int initializeWinSock();
		int createSocket();
};