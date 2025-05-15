// Main file/Entry Point
#include <iostream>
#include "Socket.h"
#include "ClientSocket.h"
#include "ServerSocket.h"
#include "SocketFactory.h"
#include "SocketType.h"
#include <memory>

int parseOption(std::string option) {
	int result = 0;
	for (char ch : option) {
		if (!isdigit(ch)) {
			return -1;
		}
		result = (result * 10) + (int)ch - '0';
	}
	return result;
}

int main() {


	while (true) {
		std::shared_ptr<Socket> socket = nullptr;
		std::string option;
		std::cout << "1. Server -> Receive Files" << std::endl;
		std::cout << "2. Client -> Send Files" << std::endl;
		std::cout << "Enter an option : ";
		std::cin >> option;
		int parsedOption = parseOption(option);
		switch (parsedOption) {
			case 1:
				socket = SocketFactory::getSocket(SocketType::SERVER);
				break;
			case 2:
				socket = SocketFactory::getSocket(SocketType::CLIENT);
				break;
			default:
				std::cout << "Invalid option" << std::endl;
				continue;
		}
		socket->setup();
	}
	return 0;
}
