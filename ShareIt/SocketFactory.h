#pragma once
#include "Socket.h"
#include "ClientSocket.h"
#include "ServerSocket.h"
#include "SocketType.h"
#include <memory>
class SocketFactory {
	public :
		static std::unique_ptr<Socket> getSocket(SocketType socketType);
};