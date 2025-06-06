#include "ServerSocket.h"
#include "FileHandler.h"
#include <queue>

int ServerSocket::setup() {
	int result;
	result = prepareSocket();
	if (result == 1)	return result;
	result = bindPort();
	if (result == 1)	return result;
	result = listenForClients();
	if (result == 1)	return result;
	std::cout << "Waiting for a client..." << std::endl;
	result = acceptClient();
	if (result == 1)	return result;

	result = startReceivingFiles();
	if (result == 1)	return result;

	std::cout << "Closing server...";

	closeAndCleanup(&tcpSocket, &clientSocket);

	return 0;
}

int ServerSocket::startReceivingFiles() {
	bool clientReady = false;

	while (true) {

		if (!clientReady) {
			std::cout << "Waiting for handshake..." << std::endl;
			clientReady = handshake();
			continue;
		}

		std::cout << "Waiting to receive a file..." << std::endl;

		std::string fileName;
		bool permission = requestPermission(fileName);
		if (permission == false) {
			if (fileName == "EXIT_FLAG") {
				std::cout << "Client has exited..." << std::endl;
				break;		// could call acceptClient from here to let the server wait for another client
			}
			if (closeServer()) {
				break;
			}
			continue;
		}

		int iterationCount;
		int iterationCountResult = getIterationCount(iterationCount);
		if (iterationCountResult == 1)	return iterationCountResult;

		int recFileResult = receiveFile(iterationCount, fileName);
		if (recFileResult == 1)	return recFileResult;

		std::cout << "File copied successfully!! (Happy noises)" << std::endl;
	}

	return 0;
}

bool ServerSocket::handshake() {
	std::string message = "READY";
	const int msgSize = 5;
	bool sendResult = sendCharPtr(&clientSocket, &tcpSocket, message.c_str(), msgSize, "Error in sending handshake");
	if (!sendResult) {
		return sendResult;
	}
	std::vector<char> messageBuff(msgSize);
	bool recvResult = recvCharPtr(&clientSocket, &tcpSocket, messageBuff.data(), msgSize, "Error in receiving handshake");
	if (!recvResult) {
		return false;
	}
	std::string recMessage(messageBuff.begin(), messageBuff.end());
	return recMessage == message;
}

int ServerSocket::receiveFile(int iterationCount, std::string fileName) {
	std::string file = getFilePath() + "/" + fileName;
	int fileWritingRes = FileHandler::writeFile(iterationCount, this, file);
	if (fileWritingRes == 1) {
		return fileWritingRes;
	}
	return 0;
}

int ServerSocket::getIterationCount(int& iterationCount) {
	return recvCharPtr(&clientSocket, &tcpSocket, (char*) &iterationCount, sizeof(int), "Error while receiving the number of chunks") ? 0 : 1;
}

int ServerSocket::bindPort() {
	sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(12345);

	int bindResult = bind(tcpSocket, (sockaddr*) &serverAddress, sizeof(serverAddress));
	return checkForError(&tcpSocket, bindResult, "Error in binding to the server");
}

int ServerSocket::listenForClients() {
	int listenResult = listen(tcpSocket, SOMAXCONN);
	return checkForError(&tcpSocket, listenResult, "Error in listening for clients");
}

int ServerSocket::acceptClient() {
	sockaddr_in clientAddress{};
	int sizeOfClientAddress = sizeof(clientAddress);
	clientSocket = accept(tcpSocket, (sockaddr*) &clientAddress, &sizeOfClientAddress);
	return checkForError(&tcpSocket, &clientSocket, "Error in accepting a client");
}

std::string ServerSocket::getFilePath() 
{
	return "../ServerFiles";
}

int ServerSocket::retrieveChunks(std::vector<char>* chunk) {
	int chunkSize;
	bool recvChunkSize = recvCharPtr(&clientSocket, &tcpSocket, (char*) &chunkSize, sizeof(chunkSize), "Error in receiving chunk size");
	if (!recvChunkSize) {
		return 1;
	}
	chunk->resize(chunkSize);
	return recvCharPtr(&clientSocket, &tcpSocket, chunk->data(), chunkSize, "Error in receiving chunk");
}

bool ServerSocket::requestPermission(std::string& fileName) {
	int msgSize;
	int receiveMsgSize = recv(clientSocket, (char*) &msgSize, sizeof(msgSize), 0);
	std::vector<char> messageBuff(msgSize);
	bool recvMessage = recvCharPtr(&clientSocket, &tcpSocket, messageBuff.data(), msgSize, "Error in receiving request message");
	if (!recvMessage)	return false;
	std::string message(messageBuff.begin(), messageBuff.end());
	if (isExitMessage(message)) {
		fileName = "EXIT_FLAG";
		return false;
	}
	std::cout << message << std::endl;
	std::cout << "Want to receive? (Y/N) : ";
	std::string response;
	std::cin >> response;
	if (toupper(response[0]) != 'Y') {
		std::string responseMessage = "NO";
		bool denyFileShare = sendCharPtr(&clientSocket, &tcpSocket, responseMessage.c_str(), responseMessage.size(), "Error in sending response");
		return false;
	}
	std::string responseMessage = "OK";
	std::string file = message;
	fileName = file.substr(21);
	return sendCharPtr(&clientSocket, &tcpSocket, responseMessage.c_str(), responseMessage.size(), "Error in sending response");
}

bool ServerSocket::isExitMessage(std::string message) {
	return message == "EXIT_FLAG";
}

bool ServerSocket::closeServer() {
	std::string response;
	std::cout << "Do you want to close the server? (Y/N) : ";
	std::cin >> response;

	return toupper(response[0]) != 'N';
}