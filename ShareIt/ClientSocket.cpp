#include "ClientSocket.h"
#include "FileHandler.h"
#include <filesystem>
#include <cctype>


int ClientSocket::setup() {
	int result;
	result = prepareSocket();
	if (result == 1)		return 1;
	/*result = bindPort();				// Optional most probably will cause an error
	if (result == 1)	return 1;*/
	result = connectToServer("127.0.0.1");		// Default IP adress for loopback
	if (result == 1)	return 1;

	// Gotta write methods to get all the files, display it, send it and an exit option!

	vector<string> files = FileHandler::getFiles(getFilePath());

	bool readyToShare = false;

	while (true) {
		
		while (!readyToShare) {
			cout << "Waiting for handshake..." << endl;
			readyToShare = handshake();
		}

		int option = getOption(files);


		if (option == -1 || (option > files.size())) {
			cout << "Enter a valid number or 0 to exit!" << endl;
			continue;
		}
		if (option == 0) {
			cout << "Closing everything and exiting...";
			break;
		}

		int result = sendFile(files[option - 1]);
	}

	closeAndCleanup(&tcpSocket);

	return 0;
}

int ClientSocket::getOption(vector<string> files) {
	cout << "Displaying all files..." << endl;
	int index = 1;
	for (string file : files) {
		cout << index++ << ". " << file << endl;
	}
	cout << "0. Exit" << endl;
	cout << "Enter the number of the file : ";
	string option;
	cin >> option;

	return parseOption(option);
}

bool ClientSocket::handshake() {
	const int msgSize = 5;
	vector<char> messageBuff(msgSize);
	bool recvResult = recvCharPtr(&tcpSocket, messageBuff.data(), msgSize, "Error in receiving handshake");
	if (!recvResult) {
		return recvResult;
	}
	string message(messageBuff.begin(), messageBuff.end());
	if (message != "READY") {
		return false;
	}
	bool sendResult = sendCharPtr(&tcpSocket, messageBuff.data(), msgSize, "Error in sending handshake");
	return sendResult;
}

int ClientSocket::bindPort() {	// Optional and might cause problems
	sockaddr_in clientAddress{};
	clientAddress.sin_family = AF_INET;
	clientAddress.sin_port = htons(54321);
	clientAddress.sin_addr.s_addr = INADDR_ANY;

	int bindResult = bind(tcpSocket, (sockaddr*) &clientAddress, sizeof(clientAddress));
	return checkForError(&tcpSocket, bindResult,  "Error in binding the client");
}

int ClientSocket::connectToServer(string serverAddr) {
	sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(12345);

	int setupServerAddress = inet_pton(AF_INET, serverAddr.c_str(), &serverAddress.sin_addr);
	if (setupServerAddress < 1) {
		cout << "Error in setting server address" << endl;
		return 1;	// returning on failure
	}
	int result = connect(tcpSocket, (sockaddr*) &serverAddress, sizeof(serverAddress));
	return checkForError(&tcpSocket, result, "Error in connecting with the server");
}

string ClientSocket::getFilePath() {
	return "../ClientFiles";
}

int ClientSocket::parseOption(string option) {
	int result = 0;
	for (char ch : option) {
		if (!isdigit(ch)) {
			return -1;
		}
		result = (result * 10) + (int) ch - '0';
	}
	return result;
}


int ClientSocket::sendFile(string fileName) {
	boolean serverResponse = sendRequest(fileName);
	cout << "Waiting for server's permission..." << endl;
	if (!serverResponse) {
		cout << "Server denied the file share request" << endl;
		return 1;
	}
	return shareFile(fileName);
}

bool ClientSocket::sendRequest(string fileName) {
	string message = "FILE SHARE REQUEST : " + fileName;
	int msgSize = message.size();
	bool sendMsgSize = sendCharPtr(&tcpSocket, (char*) &msgSize, sizeof(msgSize), "Error in sending request size");
	if (!sendMsgSize)	return false;
	bool sendMsg = sendCharPtr(&tcpSocket, message.c_str(), msgSize, "Error in sending request");
	if (!sendMsg)	return false;
	int responseSize = 2;		// OK or NO
	vector<char> responseBuff(responseSize);
	bool responseResult = recvCharPtr(&tcpSocket, responseBuff.data(), responseSize, "Error in getting response from the server");
	if (!responseResult)	return false;
	string response(responseBuff.begin(), responseBuff.end());
	return response == "OK";
}

int ClientSocket::shareFile(string fileName) {
	return FileHandler::sendFile(getFilePath() + "/" + fileName, this);
}

int ClientSocket::sendIterationCount(int count) {
	return sendCharPtr(&tcpSocket, (char*)&count, sizeof(count), "Error in sending iteration count") ? 0 : 1;
}

bool ClientSocket::sendChunk(vector<char> chunk) {
	int size = chunk.size();
	bool result = sendCharPtr(&tcpSocket, (char*) &size, sizeof(size), "Error in sending chunk size");
	if (!result) {
		return true;
	}
	bool sendResult = sendCharPtr(&tcpSocket, chunk.data(), size, "Error in sending chunk");
	return !sendResult;
}