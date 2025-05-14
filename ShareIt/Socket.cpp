#include "Socket.h"

void Socket::closeAndCleanup(SOCKET* socket1, SOCKET* socket2) {
	closesocket(*socket1);
	closesocket(*socket2);
	WSACleanup();
}

void Socket::closeAndCleanup(SOCKET* socket1) {
	closesocket(*socket1);
	WSACleanup();
}

int Socket::checkForError(SOCKET* socket1, SOCKET* socket2, int result, std::string message) {
	if (result == SOCKET_ERROR) {
		std::cout << message << ". Error code : " << WSAGetLastError() << std::endl;
		closeAndCleanup(socket1, socket2);
		return 1;
	}
	return 0;
}

int Socket::checkForError(SOCKET* socket1, int result, std::string message) {
	if (result == SOCKET_ERROR) {
		std::cout << message << ". Error code : " << WSAGetLastError() << std::endl;
		closeAndCleanup(socket1);
		return 1;
	}
	return 0;
}

int Socket::checkForError(SOCKET* socket1, SOCKET* socket2, std::string message) {
	if (*socket2 == INVALID_SOCKET) {
		std::cout << message << ". Error code : " << WSAGetLastError() << std::endl;
		closeAndCleanup(socket1);
		return 1;
	}
	return 0;
}

int Socket::checkForError(SOCKET* socket1, std::string message) {
	if (*socket1 == INVALID_SOCKET) {
		std::cout << message << ". Error code : " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	return 0;
}

int Socket::checkForError(int data, std::string message) {
	if (data != 0) {
		std::cout << message << ". Error code : " << data << std::endl;
		return 1;
	}
	return 0;
}

int Socket::initializeWinSock() {
	int ready = WSAStartup(MAKEWORD(2, 2), &wsaData);
	return checkForError(ready, "Error initializing WinSock API");
}

int Socket::createSocket() {
	tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return checkForError(&tcpSocket, "Socket creation failed");
}

int Socket::prepareSocket() {
	int result;
	result = initializeWinSock();
	if (result == 1)	return result;	// Incase initialization failed we gotta return here!
	return createSocket();
}

bool Socket::sendCharPtr(SOCKET* socket, const char* buffer, int bufferSize, std::string errorMessage) {
	int bytesSent = 0;
	while (bytesSent < bufferSize) {
		int sendResult = send(*socket, buffer + bytesSent, bufferSize - bytesSent, 0);
		int errorCheck = checkForError(socket, sendResult, errorMessage);
		if (errorCheck == 1)	return false;
		bytesSent += sendResult;
	}
	return true;
}

bool Socket::sendCharPtr(SOCKET* socket1, SOCKET* socket2, const char* buffer, int bufferSize, std::string errorMessage) {
	int bytesSent = 0;
	while (bytesSent < bufferSize) {
		int sendResult = send(*socket1, buffer + bytesSent, bufferSize - bytesSent, 0);		
		int errorCheck = checkForError(socket1, socket2, sendResult, errorMessage);
		if (errorCheck == 1)	return false;
		bytesSent += sendResult;
	}
	return true;
}

bool Socket::recvCharPtr(SOCKET* socket, char* buffer, int bufferSize, std::string errorMessage) {
	int bytesRecv = 0;
	while (bytesRecv < bufferSize) {
		int recvResult = recv(*socket, buffer + bytesRecv, bufferSize - bytesRecv, 0);
		int errorCheck = checkForError(socket, recvResult, errorMessage);
		if (errorCheck == 1)	return false;
		bytesRecv += recvResult;
	}
	return true;
}

bool Socket::recvCharPtr(SOCKET* socket1, SOCKET* socket2, char* buffer, int bufferSize, std::string errorMessage) {
	int bytesRecv = 0;
	while (bytesRecv < bufferSize) {
		int recvResult = recv(*socket1, buffer + bytesRecv, bufferSize - bytesRecv, 0);	
		int errorCheck = checkForError(socket1, socket2, recvResult, errorMessage);
		if (errorCheck == 1)	return false;
		bytesRecv += recvResult;
	}
	return true;
}