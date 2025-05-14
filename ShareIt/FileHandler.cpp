#include "FileHandler.h"
#include "FileComponent.h"
#include "FolderComponent.h"

std::mutex FileHandler::mtx;
std::condition_variable FileHandler::cv;
std::queue<std::vector<char>> FileHandler::dataBuffer;
bool FileHandler::eof = false;
bool FileHandler::error = false;

std::vector<std::string> FileHandler::getFiles(std::string filePath) {
	std::vector<std::string> files;
	try {
		for (const auto& entry : std::filesystem::directory_iterator(filePath)) {
			if (entry.is_regular_file()) {
				files.push_back(entry.path().filename().string());
			} else if (std::filesystem::is_directory(entry.status())) {
				
			}
		}
	} catch (std::filesystem::filesystem_error& e) {
		std::cout << "Something went wrong in getting the files." << std::endl;
	}
	return files;
}

// update

std::vector<std::shared_ptr<Component>> FileHandler::getComponents(std::shared_ptr<Component> component) {
	//std::shared_ptr<Component> comp = component.lock();
	std::string path = component->getPath();
	std::vector<std::shared_ptr<Component>> components;
	try {
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			if (entry.is_regular_file()) {
				components.push_back(std::make_shared<FileComponent>(entry.path().filename().string(), component));
			}
			else if (std::filesystem::is_directory(entry.status())) {
				components.push_back(std::make_shared<FolderComponent>(entry.path().filename().string(), component));
			}
		}
	}
	catch (std::filesystem::filesystem_error& e) {
		std::cout << "Something went wrong accessing the directory : " << path << std::endl;
	}
	return components;
}

int FileHandler::sendFile(std::string fileName, ClientSocket* socket) {
	eof = false;
	error = false;
	std::cout << fileName << std::endl;
	std::thread pushThread(pushFileToQueue, fileName, socket);
	std::thread sendThread(sendFileFromQueue, socket);

	pushThread.join();
	sendThread.join();

	return error ? 1 : 0;
}

void FileHandler::pushFileToQueue(std::string fileName, ClientSocket *socket) {

	std::ifstream file(fileName, std::ios::binary);
	if (!file) {
		eof = true;
		return;
	}

	const size_t chunkSize = 7168;		// 7KB
	file.seekg(0, std::ios::end);
	const size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	int totalChunks = (fileSize + chunkSize - 1) / chunkSize;
	socket->sendIterationCount(totalChunks);
	size_t remSize = fileSize;
	while (remSize > 0) {
		std::unique_lock<std::mutex> lock(mtx);
		size_t currentSize = (remSize < chunkSize) ? remSize : chunkSize;
		std::vector<char> chunk(currentSize);
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
		std::unique_lock<std::mutex> lock(mtx);
		if (error) {
			return;
		}
		cv.wait(lock, [] {return !dataBuffer.empty() || eof; });
		if (dataBuffer.empty() && eof) {
			break;
		}
		std::vector<char> chunk = dataBuffer.front();
		dataBuffer.pop();
		error = socket->sendChunk(chunk);
	}

	std::cout << "File sent successfully!" << std::endl;
}

int FileHandler::writeFile(int iterationCount, ServerSocket* socket, std::string filePath) {
	std::thread receiveThread(receiveFiles, iterationCount, socket);
	std::thread writeThread(writeFileFromBuffer, filePath);

	receiveThread.join();
	writeThread.join();

	return 0;
}

void FileHandler::receiveFiles(int iterationCount, ServerSocket* socket) {
	while (iterationCount-- > 0) 
	{
		std::unique_lock<std::mutex> lock(mtx);
		std::vector<char> chunk;
		int result = socket->retrieveChunks(&chunk);
		if (result == SOCKET_ERROR) {
			error = true;
			std::cout << "Error in receiving data" << std::endl;
			return;
		}
		dataBuffer.push(chunk);
		cv.notify_one();
	}
	eof = true;
}

void FileHandler::writeFileFromBuffer(std::string filePath) {
	std::ofstream file(filePath, std::ios::binary);
	if (!file.is_open()) {
		error = true;
		return;
	}
	while (true) 
	{
		std::unique_lock<std::mutex> lock(mtx);
		if (error) {
			return;
		}
		cv.wait(lock, [] {return !dataBuffer.empty() || eof; });
		if (dataBuffer.empty() && eof) {
			break;
		}
		std::vector<char> chunk = dataBuffer.front();
		dataBuffer.pop();
		file.write(chunk.data(), chunk.size());
	}

	std::cout << "Writing files successful!" << std::endl;
}