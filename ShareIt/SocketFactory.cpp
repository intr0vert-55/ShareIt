#include "SocketFactory.h"

std::shared_ptr<Socket> SocketFactory::getSocket(SocketType socketType) {
	if (socketType == SocketType::CLIENT) {
		return std::make_shared<ClientSocket>();
	}

	return std::make_shared<ServerSocket>();
}