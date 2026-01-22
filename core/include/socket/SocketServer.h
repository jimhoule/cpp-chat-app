#pragma once

#include "socket/SocketEventName.h"
#include "socket/SocketServerEventHandler.h"

#include <arpa/inet.h>
#include <map>
#include <string>

class SocketServer
{
  public:
	SocketServer(unsigned int Port);

	void Close();
	void Init();
	void Listen();
    void On(SocketEventName SocketEventName, const SocketServerEventHandler& SocketEventHandler);
	void Send(int ClientSocket, const std::string& SerializedSocketEvent);

  private:
    int m_Socket = 0;
    const unsigned int m_Port = 0;
	sockaddr_in m_Address = {};
    std::map<SocketEventName, SocketServerEventHandler> m_SocketEventHandlersMap = {};
};
