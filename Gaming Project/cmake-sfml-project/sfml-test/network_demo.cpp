// ================================================================
// SFML Network Demo
// Demonstrates TCP client/server communication, UDP messaging,
// non-blocking sockets, and use of sf::SocketSelector.
//
// Requires: SFML (Audio, System, Network modules)
// ================================================================

#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <thread>
#include <vector>

// ------------------------------------------------
// TCP SERVER: Waits for incoming connections and echoes messages.
// ------------------------------------------------
void runTcpServer(unsigned short port)
{
    sf::TcpListener listener;
    if (listener.listen(port) != sf::Socket::Done)
    {
        std::cerr << "[SERVER] Failed to bind port " << port << "\n";
        return;
    }

    std::cout << "[SERVER] Listening on port " << port << "...\n";

    sf::SocketSelector selector;
    selector.add(listener);

    std::vector<std::unique_ptr<sf::TcpSocket>> clients;

    while (true)
    {
        if (selector.wait(sf::seconds(0.1f))) // wait for activity
        {
            // New client connection?
            if (selector.isReady(listener))
            {
                auto client = std::make_unique<sf::TcpSocket>();
                if (listener.accept(*client) == sf::Socket::Done)
                {
                    std::cout << "[SERVER] New connection from "
                              << client->getRemoteAddress() << ":"
                              << client->getRemotePort() << "\n";

                    client->setBlocking(false);
                    selector.add(*client);
                    clients.push_back(std::move(client));
                }
            }

            // Existing client message?
            for (auto& c : clients)
            {
                if (selector.isReady(*c))
                {
                    char data[256];
                    std::size_t received;
                    sf::Socket::Status status = c->receive(data, sizeof(data), received);

                    if (status == sf::Socket::Done)
                    {
                        std::string msg(data, received);
                        std::cout << "[SERVER] Received: " << msg << "\n";

                        // Echo the message back
                        c->send(msg.c_str(), msg.size());
                    }
                    else if (status == sf::Socket::Disconnected)
                    {
                        std::cout << "[SERVER] Client disconnected.\n";
                        selector.remove(*c);
                    }
                }
            }
        }
    }
}

// ------------------------------------------------
// TCP CLIENT: Connects to server and sends user messages.
// ------------------------------------------------
void runTcpClient(const sf::IpAddress& server, unsigned short port)
{
    sf::TcpSocket socket;
    std::cout << "[CLIENT] Connecting to " << server << ":" << port << "...\n";

    if (socket.connect(server, port, sf::seconds(5)) != sf::Socket::Done)
    {
        std::cerr << "[CLIENT] Connection failed!\n";
        return;
    }

    std::cout << "[CLIENT] Connected! Type messages to send (type 'exit' to quit):\n";
    socket.setBlocking(false);

    while (true)
    {
        std::string msg;
        std::cout << "> ";
        std::getline(std::cin, msg);
        if (msg == "exit")
            break;

        if (socket.send(msg.c_str(), msg.size()) != sf::Socket::Done)
            std::cerr << "[CLIENT] Failed to send message.\n";

        char data[256];
        std::size_t received;
        if (socket.receive(data, sizeof(data), received) == sf::Socket::Done)
        {
            std::cout << "[CLIENT] Echo: " << std::string(data, received) << "\n";
        }
        sf::sleep(sf::milliseconds(100));
    }
}

// ------------------------------------------------
// UDP DEMO: Send a packet to self (loopback) and receive it.
// ------------------------------------------------
void runUdpDemo(unsigned short port)
{
    sf::UdpSocket socket;
    if (socket.bind(port) != sf::Socket::Done)
    {
        std::cerr << "[UDP] Failed to bind port " << port << "\n";
        return;
    }

    std::cout << "[UDP] Bound to port " << port << "\n";

    // Send a message to self (localhost)
    std::string message = "Hello via UDP!";
    socket.send(message.c_str(), message.size(), sf::IpAddress::LocalHost, port);

    char buffer[256];
    std::size_t received;
    sf::IpAddress sender;
    unsigned short senderPort;

    if (socket.receive(buffer, sizeof(buffer), received, sender, senderPort) == sf::Socket::Done)
    {
        std::cout << "[UDP] Received " << received << " bytes from "
                  << sender << ":" << senderPort << " -> "
                  << std::string(buffer, received) << "\n";
    }

    socket.unbind();
}

// ------------------------------------------------
// MAIN PROGRAM: Choose mode (server, client, UDP demo)
// ------------------------------------------------
int main()
{
    std::cout << "=== SFML Network Socket Demo ===\n"
              << "1. Run TCP Server\n"
              << "2. Run TCP Client\n"
              << "3. Run UDP Demo\n"
              << "Select mode: ";

    int mode = 0;
    std::cin >> mode;

    const unsigned short port = 53000;

    if (mode == 1)
    {
        runTcpServer(port);
    }
    else if (mode == 2)
    {
        runTcpClient(sf::IpAddress::LocalHost, port);
    }
    else if (mode == 3)
    {
        runUdpDemo(54000);
    }
    else
    {
        std::cout << "Invalid mode.\n";
    }

    return 0;
}