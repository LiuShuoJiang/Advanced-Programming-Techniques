The program uses the native Linux socket programming interface instead of SFML, so there is no need to install any third-party libraries.

Compile server program: g++ server.cpp -std=c++17 -lpthread -o server
Run server program: ./server

Compile client program: g++ client.cpp -std=c++17 -lpthread -o client
Run client program: ./client <IP> <PORT>
