#pragma once
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <set>
#include <queue>
#include <filesystem>
#include <iostream>
#include "ClientSocket.h"
#include "ServerSocket.h"
#include "Component.h"


class FileHandler {
	private :
		static std::mutex mtx;
		static std::condition_variable cv;
		static std::queue<std::vector<char>> dataBuffer;
		static bool eof, error;
		static void pushFileToQueue(std::string fileName, ClientSocket* socket);
		static void sendFileFromQueue(ClientSocket* socket);
		static void receiveFiles(int iterations, ServerSocket* socket);
		static void writeFileFromBuffer(std::string filePath);
	public :
		static std::vector<std::shared_ptr<Component>> getComponents(std::shared_ptr<Component> component);
		//static std::vector<std::string> getFiles(std::string filePath);
		static int sendFile(std::string fileName, ClientSocket* socket);
		static int writeFile(int iterations, ServerSocket* socket, std::string filePath);
};