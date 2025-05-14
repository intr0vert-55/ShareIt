// Main file/Entry Point
#include <iostream>
#include "Socket.h"
#include "ClientSocket.h"
#include "ServerSocket.h"

int parseOption(string option) {
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

	while (true) 
	{
		Socket* socket = NULL;
		string option;
		cout << "1. Server -> Receive Files" << endl;
		cout << "2. Client -> Send Files" << endl;
		cout << "Enter an option : ";
		cin >> option;
		int parsedOption = parseOption(option);
		switch (parsedOption) {
			case 1:
				socket = new ServerSocket();
				break;
			case 2:
				socket = new ClientSocket();
				break;
			default:
				cout << "Invalid option" << endl;
				continue;
		}
		socket->setup();
	}
	return 0;
}
