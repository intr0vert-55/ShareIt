#pragma once
#include "Socket.h"
#include <vector>

class ClientSocket : public Socket {
	public :
		int setup() override;
		bool sendChunk(vector<char> chunk);
		int sendIterationCount(int count);
	private :
		int bindPort();
		int connectToServer(string serverAddr);
		string getFilePath() override;
		int parseOption(string option);
		int sendFile(string filename);
		bool sendRequest(string filename);	// helper methods for sendFile (send & sendRequest)
		int shareFile(string filename);
		bool handshake();
		int getOption(vector<string> files);
};