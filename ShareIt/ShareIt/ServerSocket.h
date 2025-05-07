// This is the server socket and server should be active first in order to receive data!
#pragma once
#include "Socket.h"
#include <vector>

class ServerSocket : public Socket{
	public :
		int setup() override;
		int receiveFile(int iterations, string fileName);
		int retrieveChunks(vector<char>* chunk);
	private :
		SOCKET clientSocket;	// client socket
		int bindPort();
		int listenForClients();
		int acceptClient();
		string getFilePath() override;
		bool requestPermission(string& fileName);
		bool closeServer();
		int getIterationCount(int& iterationCount);
		bool handshake();
};