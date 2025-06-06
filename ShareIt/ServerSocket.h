// This is the server socket and server should be active first in order to receive data!
#pragma once
#include "Socket.h"
#include <vector>
class ServerSocket : public Socket {
	public :
		int setup() override;
		int receiveFile(int iterations, std::string fileName);
		int retrieveChunks(std::vector<char>* chunk);
	private :
		SOCKET clientSocket;	// client socket
		int bindPort();
		int listenForClients();
		int acceptClient();
		int startReceivingFiles();
		std::string getFilePath() override;
		bool requestPermission(std::string& fileName);
		bool isExitMessage(std::string message);
		bool closeServer();
		int getIterationCount(int& iterationCount);
		bool handshake();
};