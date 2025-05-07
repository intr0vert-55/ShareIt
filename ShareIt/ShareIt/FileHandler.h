#pragma once
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <filesystem>
#include <iostream>
#include "ClientSocket.h"
#include "ServerSocket.h"
using namespace std;

class FileHandler {
	private :
		static mutex mtx;
		static condition_variable cv;
		static queue<vector<char>> dataBuffer;
		static bool eof, error;
		static void pushFileToQueue(string fileName, ClientSocket* socket);
		static void sendFileFromQueue(ClientSocket* socket);
		static void receiveFiles(int iterations, ServerSocket* socket);
		static void writeFileFromBuffer(string filePath);
	public :
		static vector<string> getFiles(string filePath);
		static int sendFile(string fileName, ClientSocket* socket);
		static int writeFile(int iterations, ServerSocket* socket, string filePath);
};