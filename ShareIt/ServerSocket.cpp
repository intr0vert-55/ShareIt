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
	cout << "Waiting for a client..." << endl;
	result = acceptClient();
	if (result == 1)	return result;

	bool clientReady = false;

	while (true) {

		if (!clientReady) {
			cout << "Waiting for handshake..." << endl;
			clientReady = handshake();
			continue;
		}

		cout << "Waiting to receive a file..." << endl;

		string fileName;
		bool permission = requestPermission(fileName);
		if (permission == false) {
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

		cout << "File copied successfully!! (Happy noises)" << endl;
	}

	cout << "Closing server...";

	closeAndCleanup(&tcpSocket, &clientSocket);

	return 0;
}

bool ServerSocket::handshake() {
	string message = "READY";
	const int msgSize = 5;
	bool sendResult = sendCharPtr(&clientSocket, &tcpSocket, message.c_str(), msgSize, "Error in sending handshake");
	if (!sendResult) {
		return sendResult;
	}
	vector<char> messageBuff(msgSize);
	bool recvResult = recvCharPtr(&clientSocket, &tcpSocket, messageBuff.data(), msgSize, "Error in receiving handshake");
	if (!recvResult) {
		return false;
	}
	string recMessage(messageBuff.begin(), messageBuff.end());
	return recMessage == message;
}

int ServerSocket::receiveFile(int iterationCount, string fileName) {
	string file = getFilePath() + "/" + fileName;
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

string ServerSocket::getFilePath() 
{
	return "../ServerFiles";
}

int ServerSocket::retrieveChunks(vector<char>* chunk) {
	int chunkSize;
	bool recvChunkSize = recvCharPtr(&clientSocket, &tcpSocket, (char*) &chunkSize, sizeof(chunkSize), "Error in receiving chunk size");
	if (!recvChunkSize) {
		return 1;
	}
	chunk->resize(chunkSize);
	return recvCharPtr(&clientSocket, &tcpSocket, chunk->data(), chunkSize, "Error in receiving chunk");
}

bool ServerSocket::requestPermission(string& fileName) {
	int msgSize;
	int receiveMsgSize = recv(clientSocket, (char*) &msgSize, sizeof(msgSize), 0);
	vector<char> messageBuff(msgSize);
	bool recvMessage = recvCharPtr(&clientSocket, &tcpSocket, messageBuff.data(), msgSize, "Error in receiving request message");
	if (!recvMessage)	return false;
	string message(messageBuff.begin(), messageBuff.end());
	cout << message << endl;
	cout << "Want to receive? (Y/N) : ";
	string response;
	cin >> response;
	if (toupper(response[0]) != 'Y') {
		string responseMessage = "NO";
		bool denyFileShare = sendCharPtr(&clientSocket, &tcpSocket, responseMessage.c_str(), responseMessage.size(), "Error in sending response");
		return false;
	}
	string responseMessage = "OK";
	string file = message;
	fileName = file.substr(21);
	return sendCharPtr(&clientSocket, &tcpSocket, responseMessage.c_str(), responseMessage.size(), "Error in sending response");
}

bool ServerSocket::closeServer() {
	string response;
	cout << "Do you want to close the server? (Y/N) : ";
	cin >> response;

	return toupper(response[0]) != 'N';
}