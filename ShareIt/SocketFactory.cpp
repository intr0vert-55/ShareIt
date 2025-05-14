#include "SocketFactory.h"

std::unique_ptr<Socket> SocketFactory::getSocket(SocketType socketType) {
	if (socketType == SocketType::CLIENT) {
		return std::make_unique<ClientSocket>();
	}

	return std::make_unique<ServerSocket>();
}