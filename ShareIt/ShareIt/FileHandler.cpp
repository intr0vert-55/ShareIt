#include "FileHandler.h"

mutex FileHandler::mtx;
condition_variable FileHandler::cv;
queue<vector<char>> FileHandler::dataBuffer;
bool FileHandler::eof = false;
bool FileHandler::error = false;

vector<string> FileHandler::getFiles(string filePath) {
	vector<string> files;
	try {
		for (const auto& entry : filesystem::directory_iterator(filePath)) {
			if (entry.is_regular_file()) {
				files.push_back(entry.path().filename().string());
			}
		}
	}
	catch (filesystem::filesystem_error& e) {
		cout << "Something went wrong in getting the files." << endl;
	}
	return files;
}

int FileHandler::sendFile(string fileName, ClientSocket* socket) {
	eof = false;
	error = false;
	thread pushThread(pushFileToQueue, fileName, socket);
	thread sendThread(sendFileFromQueue, socket);

	pushThread.join();
	sendThread.join();

	return error ? 1 : 0;
}

void FileHandler::pushFileToQueue(string fileName, ClientSocket *socket) {

	ifstream file(fileName, ios::binary);
	if (!file) {
		eof = true;
		return;
	}

	const size_t chunkSize = 7168;		// 7KB
	file.seekg(0, ios::end);
	const size_t fileSize = file.tellg();
	file.seekg(0, ios::beg);
	int totalChunks = (fileSize + chunkSize - 1) / chunkSize;
	socket->sendIterationCount(totalChunks);
	size_t remSize = fileSize;
	while (remSize > 0) {
		unique_lock<mutex> lock(mtx);
		size_t currentSize = (remSize < chunkSize) ? remSize : chunkSize;
		vector<char> chunk(currentSize);
		file.read(chunk.data(), currentSize);
		dataBuffer.push(chunk);
		remSize -= currentSize;
		cv.notify_one();
	}
	eof = true;
	cv.notify_one();
}

void FileHandler::sendFileFromQueue(ClientSocket* socket) {
	while (true) {
		unique_lock<mutex> lock(mtx);
		if (error) {
			return;
		}
		cv.wait(lock, [] {return !dataBuffer.empty() || eof; });
		if (dataBuffer.empty() && eof) {
			break;
		}
		vector<char> chunk = dataBuffer.front();
		dataBuffer.pop();
		error = socket->sendChunk(chunk);
	}

	cout << "File sent successfully!" << endl;
}

int FileHandler::writeFile(int iterationCount, ServerSocket* socket, string filePath) {
	thread receiveThread(receiveFiles, iterationCount, socket);
	thread writeThread(writeFileFromBuffer, filePath);

	receiveThread.join();
	writeThread.join();

	return 0;
}

void FileHandler::receiveFiles(int iterationCount, ServerSocket* socket) {
	while (iterationCount-- > 0) 
	{
		unique_lock<mutex> lock(mtx);
		vector<char> chunk;
		int result = socket->retrieveChunks(&chunk);
		if (result == SOCKET_ERROR) {
			error = true;
			cout << "Error in receiving data" << endl;
			return;
		}
		dataBuffer.push(chunk);
		cv.notify_one();
	}
	eof = true;
}

void FileHandler::writeFileFromBuffer(string filePath) {
	ofstream file(filePath, ios::binary);
	if (!file.is_open()) {
		error = true;
		return;
	}
	while (true) 
	{
		unique_lock<mutex> lock(mtx);
		if (error) {
			return;
		}
		cv.wait(lock, [] {return !dataBuffer.empty() || eof; });
		if (dataBuffer.empty() && eof) {
			break;
		}
		vector<char> chunk = dataBuffer.front();
		dataBuffer.pop();
		file.write(chunk.data(), chunk.size());
	}

	cout << "Writing files successful!" << endl;
}