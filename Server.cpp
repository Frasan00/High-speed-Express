#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
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

std::vector<std::string> httpMethods = { "GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "TRACE", "CONNECT" };
std::vector<std::string> httpVersions = { "HTTP/1.0", "HTTP/1.1", "HTTP/2.0" };

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
            const std::string data = buffer;
            if(this->isValidPackage(buffer, clientSocket) == false){
                std::cout << "Received invalid HTTP packet" << "\n";
                return;
            }
            std::cout << data << "\n";
        }

        std::vector<std::string> splitString(const char* str, char delimiter){
            std::vector<std::string> tokens;

            char* copy = new char[strlen(str) + 1];
            strcpy(copy, str);
            
            char* token = strtok(copy, &delimiter);
            
            while (token != nullptr){
                tokens.push_back(token);
                token = strtok(nullptr, &delimiter);
            }
            delete[] copy;
            
            return tokens;
        }

        bool isValidPackage(char* buffer, int clientSocket) {
            if (strlen(buffer) == 0) { return false; }

            std::vector<std::string> lines = this->splitString(buffer, '\n');
            if (lines.size() < 1) { return false; }

            std::string firstLine = lines[0];
            std::vector<std::string> splittedFirstLine = this->splitString(firstLine.c_str(), ' ');
            if (splittedFirstLine.size() < 2) { return false; }

            std::string method = splittedFirstLine[0];
            std::string version = splittedFirstLine[2];
            version.pop_back();

            if (std::find(httpMethods.begin(), httpMethods.end(), method) == httpMethods.end() ||
                std::find(httpVersions.begin(), httpVersions.end(), version) == httpVersions.end()) { return false; }
            
            return true;
        }

};

#endif
