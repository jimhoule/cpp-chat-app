#include "SocketClient.h"

#include <arpa/inet.h>
#include <string>
#include <unistd.h>

constexpr int SERVER_PORT =  5000;

int main()
{
    SocketClient socket_client;
    socket_client.Connect(SERVER_PORT, "127.0.0.1");

    sleep(1);

    std::string message = "Hello from Client Socket!";
    socket_client.Send(message);
    socket_client.Close();
}
