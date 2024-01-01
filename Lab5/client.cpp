/*
 * Author: Shuojiang Liu
 * Class: ECE6122
 * Last Date Modified: Nov 21, 2023
 * Description:
 * This is the client side of the program.
 * This program does not use SFML, instead, it uses Linux socket programming library.
 * Compile with: g++ client.cpp -std=c++17 -lpthread -o client
 * (Make sure to add -std=c++17 and -lpthread flags on PACE!)
 * Run with: ./client <IP Address> <Port Number>
 */

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>

// The data being sent back and forth
struct tcpMessage
{
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

std::mutex lastMessageMutex;

// Use another thread to receive messages from the server
void receiveMessage(int clientSocket)
{
    tcpMessage message;
    while (true)
    {
        int bytesReceived = recv(clientSocket, &message, sizeof(message), 0);
        if (bytesReceived < 0)
        {
            std::cerr << "Error receiving message" << std::endl;
            break;
        }
        else if (bytesReceived == 0)
        {
            lastMessageMutex.lock();
            std::cout << "Server disconnected" << std::endl;
            lastMessageMutex.unlock();
            break;
        }
        else
        {
            lastMessageMutex.lock();
            std::cout << "Received Msg Type: " << (int)message.nType << "; Msg: " << message.chMsg << std::endl;
            lastMessageMutex.unlock();
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <IP Address> <Port Number>" << std::endl;
        return -1;
    }

    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // Create a sockaddr_in struct for the server
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[2]));

    // if argv[1] inputs "localhost", convert it into 127.0.0.1
    if (strcmp(argv[1], "localhost") == 0)
    {
        serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    }
    else if (inet_addr(argv[1]) == -1)
    {
        std::cerr << "Invalid IP Address" << std::endl;
        return -1;
    }
    else
    {
        serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        std::cerr << "Error connecting to server" << std::endl;
        return -1;
    }

    // Create a thread to receive messages from the server
    std::thread receiveThread(receiveMessage, clientSocket);

    std::string command;
    tcpMessage message;
    message.nVersion = 102;
    while (true)
    {
        std::cout << "Please enter command: ";
        std::getline(std::cin, command);
        if (command == "q")
        {
            break;
        }
        else if (command[0] == 'v')
        {
            // The command is: v [number]
            message.nVersion = atoi(command.substr(2).c_str());
        }
        else if (command[0] == 't')
        {
            // The command is: t [number] [message string]
            message.nType = atoi(command.substr(2, command.find(' ', 2) - 2).c_str());
            strcpy(message.chMsg, command.substr(command.find(' ', 2) + 1).c_str());
            message.nMsgLen = strlen(message.chMsg);
            send(clientSocket, &message, sizeof(message), 0);
        }
        else
        {
            std::cout << "Invalid command" << std::endl;
        }
    }

    // Wait for the receiving thread to finish
    receiveThread.detach();

    // Close the socket
    close(clientSocket);

    return 0;
}
