#pragma once
#include "Socket.h"
#include "ClientSocket.h"
#include "ServerSocket.h"
#include "SocketType.h"
#include <memory>
class SocketFactory {
	public :
		static std::shared_ptr<Socket> getSocket(SocketType socketType);
};