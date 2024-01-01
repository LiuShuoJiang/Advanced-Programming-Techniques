/*
 * Author: Shuojiang Liu
 * Class: ECE6122
 * Last Date Modified: Nov 21, 2023
 * Description:
 * This is the server side of the program.
 * This program does not use SFML, instead, it uses Linux socket programming library.
 * Compile with: g++ server.cpp -std=c++17 -lpthread -o server
 * (Make sure to add -std=c++17 and -lpthread flags on PACE!)
 * Run with: ./server
 */

#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

const int MAX_CLIENT = 100;

// The data being sent back and forth
struct tcpMessage
{
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

std::vector<int> clientSockets;
std::mutex clientListMutex, lastMessageMutex;
tcpMessage lastMessage;
bool isLastMessageAvailable = false;

// The program continuously prompt the user for commands to execute
void handleUserInput()
{
    std::string command;

    while (true)
    {
        std::cout << "Please enter command: ";
        std::getline(std::cin, command);

        if (command == "msg")
        {
            // Print the last message received
            lastMessageMutex.lock();
            if (isLastMessageAvailable)
            {
                std::cout << "Last Message: " << lastMessage.chMsg << std::endl;
            }
            else
            {
                std::cout << "No message received yet." << std::endl;
            }
            lastMessageMutex.unlock();
        }
        else if (command == "clients")
        {
            // Print all connected clients
            clientListMutex.lock();
            std::cout << "Number of Clients: " << clientSockets.size() << std::endl;
            for (int client : clientSockets)
            {
                struct sockaddr_in clientAddr;
                socklen_t clientAddrLen = sizeof(clientAddr);
                getpeername(client, (struct sockaddr *)&clientAddr, &clientAddrLen);
                std::cout << "IP Address: " << inet_ntoa(clientAddr.sin_addr)
                          << " | Port: " << ntohs(clientAddr.sin_port) << std::endl;
            }
            clientListMutex.unlock();
        }
        else if (command == "exit")
        {
            // Close all sockets and terminate the program
            clientListMutex.lock();
            for (int client : clientSockets)
            {
                close(client);
            }
            clientListMutex.unlock();
            exit(EXIT_SUCCESS);
        }
        else
        {
            std::cout << "Invalid command" << std::endl;
        }
    }
}

// Handle the client connection
void handleClient(int clientSocket)
{
    tcpMessage message;
    while (true)
    {
        memset(&message, 0, sizeof(message));
        int readSize = read(clientSocket, &message, sizeof(message));
        if (readSize == 0)
        {
            std::cout << "Client " << clientSocket << " disconnected" << std::endl;
            break;
        }
        else if (readSize < 0)
        {
            std::cerr << "read error" << std::endl;
            break;
        }

        // update the last message
        lastMessageMutex.lock();
        lastMessage = message;
        isLastMessageAvailable = true;
        lastMessageMutex.unlock();

        if (message.nVersion != 102)
        {
            std::cout << "Invalid version, message ignored" << std::endl;
            continue;
        }

        if (message.nType == 77)
        {
            std::cout << "Broadcasting message: " << message.chMsg << std::endl;
            clientListMutex.lock();
            for (int client : clientSockets)
            {
                if (client != clientSocket)
                {
                    write(client, &message, sizeof(message));
                }
            }
            clientListMutex.unlock();
        }
        else if (message.nType == 201)
        {
            std::cout << "Reversing message: " << message.chMsg << std::endl;
            std::reverse(message.chMsg, message.chMsg + message.nMsgLen);
            write(clientSocket, &message, sizeof(message));
        }
        else
        {
            std::cout << "Invalid type" << std::endl;
        }
    }
    close(clientSocket);
    clientListMutex.lock();
    // Remove the client from the list
    clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), clientSocket), clientSockets.end());
    clientListMutex.unlock();
}

int main()
{
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    int serverFd;

    if (serverFd = socket(AF_INET, SOCK_STREAM, 0); serverFd < 0)
    {
        std::cerr << "socket error" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "bind error" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(serverFd, MAX_CLIENT) < 0)
    {
        std::cerr << "listen error" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on " << inet_ntoa(serverAddr.sin_addr) << ":" << ntohs(serverAddr.sin_port)
              << std::endl;

    int clientSocket;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Create a thread for command handling
    std::thread userInputThread(handleUserInput);

    while (true)
    {
        if (clientSocket = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen); clientSocket < 0)
        {
            std::cerr << "accept error" << std::endl;
            break;
        }
        std::cout << "New connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port)
                  << std::endl;
        clientListMutex.lock();
        clientSockets.push_back(clientSocket);
        clientListMutex.unlock();
        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    userInputThread.join();
    close(serverFd);

    return 0;
}
