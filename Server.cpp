#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

/*
* Main process for a server socket: creation, binding to (ip, port), listen, connection
*/

class Server{
    public:
        Server(int port, int max_connections){
            this->port = port;
            this->max_connections = max_connections;
        }

        void start(){
            // server socket creation
            serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            if(serverSocket == -1) {
                std::cerr << "Failed to create server socket." << "\n";
                return;
            }
            std::cout << "Socket created succesfully" << "\n";

            // binding the socket to (addr, port)
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_addr.s_addr = INADDR_ANY;
            serverAddress.sin_port  = htons(port);
            int bindSocket = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
            if(bindSocket == -1){
                std::cerr << "Failed to bind the socket" << "\n";
                close(serverSocket);
                return;
            }

            int listenSocket = listen(serverSocket, max_connections);
            if(listenSocket == -1){
                std::cerr << "Failed to listen for incoming connections" << "\n";
                close(serverSocket);
                return;
            }

            // passive listen for incoming connections
            while(true){
                clientAddressLength = sizeof(clientAddress);
                clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);

                if (clientSocket == -1) {
                    std::cerr << "Failed to accept connection." << "\n";
                    close(serverSocket);
                    return;
                }

                // client connected
                std::cout << "Client connected, socket: "+clientSocket << "\n";

                // handle incoming message
                const int bufferSize = 1024;
                char buffer[bufferSize];
                memset(buffer, 0, bufferSize);

                ssize_t bytesRead = recv(clientSocket, buffer, bufferSize - 1, 0);
                if (bytesRead == -1) {std::cerr << "Failed to read data from client." << "\n"; } 
                else if (bytesRead == 0) { std::cout << "Client disconnected. Socket: " << clientSocket << "\n"; } 
                else {
                    // Data received successfully
                    std::cout << "Data received from socket: "+clientSocket << "\n";
                    this->handleData(serverSocket, clientSocket, buffer);
                }
                
                // Close the client socket
                close(clientSocket);
                std::cout << "Client disconnected, socket: " << clientSocket << "\n";
            }
        }
    
    private:
        int serverSocket, clientSocket, port, max_connections;
        struct sockaddr_in serverAddress, clientAddress;
        socklen_t clientAddressLength;

        void handleData(int serverSocket, int clientSocket, char* buffer){
            std::cout << buffer << "\n"; // to delete
        }
};

#endif
