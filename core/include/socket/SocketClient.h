#pragma once

#include "socket/SocketEvent.h"
#include "socket/SocketClientEventHandler.h"

#include <arpa/inet.h>
#include <map>
#include <string>

class SocketClient
{
  public:
	SocketClient() = default;

	void Close();
	void Connect(int ServerPort, const std::string &ServerIpAddress);
    void On(SocketEventName SocketEventName, const SocketClientEventHandler& SocketEventHandler);
    void Read();
	void Send(const std::string &SerializedSocketEvent);

  private:
	int m_Socket = 0;
    std::map<SocketEventName, SocketClientEventHandler> m_SocketEventHandlersMap = {};
};
