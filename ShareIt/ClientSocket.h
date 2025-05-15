#pragma once
#include "Socket.h"
#include "Component.h"
#include "FolderComponent.h"
#include <vector>
#include <memory>
class ClientSocket : public Socket {
	public :
		int setup() override;
		bool sendChunk(std::vector<char> chunk);
		int sendIterationCount(int count);
	private :
		int bindPort();
		int connectToServer(std::string serverAddr);
		int startSendingFiles();
		std::string getFilePath() override;
		int parseOption(std::string option);
		void sendExitFlag();
		int sendFile(std::shared_ptr<Component> component);
		bool sendRequest(std::string filename);	// helper methods for sendFile (send & sendRequest)
		int shareFile(std::string filename);
		bool handshake();
		int getOption(std::vector<std::string> files);
};