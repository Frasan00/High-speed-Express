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
#include <unordered_map>
#include <functional>
#include <cstring>
#include "Request.cpp"
#include "Response.cpp"

/*
* Main process for a server socket: creation, binding to (ip, port), listen, connection
*/

std::vector<std::string> httpMethods = { "GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "TRACE", "CONNECT" };
std::vector<std::string> httpVersions = { "HTTP/1.0", "HTTP/1.1", "HTTP/2.0" };
typedef std::function<void(Request* req, Response* res)> FunctionType;

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
                std::cerr << "Failed to create server socket." << std::endl;
                return;
            }
            std::cout << "Socket created succesfully" << std::endl;
            int opt=1;
            socklen_t optlen=sizeof(opt);

            // binding the socket to (addr, port)
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_addr.s_addr = INADDR_ANY;
            serverAddress.sin_port  = htons(port);
            setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, optlen);
            int bindSocket = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
            if(bindSocket == -1){
                std::cerr << "Failed to bind the socket" << std::endl;
                close(serverSocket);
                return;
            }

            int listenSocket = listen(serverSocket, max_connections);
            if(listenSocket == -1){
                std::cerr << "Failed to listen for incoming connections" << std::endl;
                close(serverSocket);
                return;
            }

            // passive listen for incoming connections
            while(true){
                clientAddressLength = sizeof(clientAddress);
                clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);

                if (clientSocket == -1) {
                    std::cerr << "Failed to accept connection." << std::endl;
                    close(serverSocket);
                    return;
                }

                // client connected
                std::cout << "Client connected, socket: "+clientSocket << std::endl;

                // handle incoming message
                const int bufferSize = 1024;
                char buffer[bufferSize];
                memset(buffer, 0, bufferSize);

                ssize_t bytesRead = recv(clientSocket, buffer, bufferSize - 1, 0);
                if (bytesRead == -1) { std::cerr << "Failed to read data from client." << std::endl; } 
                else if (bytesRead == 0) { std::cout << "Client disconnected. Socket: " << clientSocket << std::endl; } 
                else {
                    // Data received successfully
                    std::cout << "Data received from socket: "+clientSocket << std::endl;
                    this->handleData(serverSocket, clientSocket, buffer);
                }
                
                // Close the client socket
                /*close(clientSocket);
                std::cout << "Client disconnected, socket: " << clientSocket << std::endl;*/
            }
        }

        // simple handlers
        void use(std::string path, FunctionType handler){
            if (useHandlers.count(path) > 0) {
                return;
            }
            useHandlers[path].push_back(handler);
        }

        void get(std::string path, FunctionType handler){
            if (getHandlers.count(path) > 0) {
                return;
            }
            getHandlers[path].push_back(handler);
        }

        void post(std::string path, FunctionType handler){
            if (postHandlers.count(path) > 0) {
                return;
            }
            postHandlers[path].push_back(handler);
        }

        void patch(std::string path, FunctionType handler){
            if (patchHandlers.count(path) > 0) {
                return;
            }
            patchHandlers[path].push_back(handler);
        }

        void del(std::string path, FunctionType handler){
            if (deleteHandlers.count(path) > 0) {
                return;
            }
            deleteHandlers[path].push_back(handler);
        }

        // handlers with middlewares
        void use(std::string path, std::vector<FunctionType> middlewares, FunctionType handler){
            if (useHandlers.count(path) > 0) {
                return;
            }
            for(const auto& middleware : middlewares) useHandlers[path].push_back(middleware);
            useHandlers[path].push_back(handler);
        }

        void get(std::string path, std::vector<FunctionType> middlewares, FunctionType handler){
            if (getHandlers.count(path) > 0) {
                return;
            }
            for(const auto& middleware : middlewares) getHandlers[path].push_back(middleware);
            getHandlers[path].push_back(handler);
        }

        void post(std::string path, std::vector<FunctionType> middlewares, FunctionType handler){
            if (postHandlers.count(path) > 0) {
                return;
            }
            for(const auto& middleware : middlewares) postHandlers[path].push_back(middleware);
            postHandlers[path].push_back(handler);
        }

        void patch(std::string path, std::vector<FunctionType> middlewares, FunctionType handler){
            if (patchHandlers.count(path) > 0) {
                return;
            }
            for(const auto& middleware : middlewares) patchHandlers[path].push_back(middleware);
            patchHandlers[path].push_back(handler);
        }

        void del(std::string path, std::vector<FunctionType> middlewares, FunctionType handler){
            if (deleteHandlers.count(path) > 0) {
                return;
            }
            for(const auto& middleware : middlewares) deleteHandlers[path].push_back(middleware);
            deleteHandlers[path].push_back(handler);
        }
    
    private:
        int serverSocket, clientSocket, port, max_connections;
        struct sockaddr_in serverAddress, clientAddress;
        socklen_t clientAddressLength;
        std::unordered_map<std::string, std::vector<FunctionType>> useHandlers, getHandlers, postHandlers, patchHandlers, deleteHandlers;

        void handleData(int serverSocket, int clientSocket, char* buffer){
            std::vector<std::string> lines = this->splitString(buffer, '\n');
            std::string firstLine = lines[0];
            std::vector<std::string> splittedFirstLine = this->splitString(firstLine.c_str(), ' ');

            // First packet check
            if (strlen(buffer) == 0 || lines.size() < 1 || splittedFirstLine.size() < 2) { 
                std::cout << "Received invalid HTTP packet" << std::endl;
                return;
            }

            std::string method = splittedFirstLine[0];
            std::string path = splittedFirstLine[1];
            std::string version = splittedFirstLine[2];
            version.pop_back();

            // second packet check
            if (std::find(httpMethods.begin(), httpMethods.end(), method) == httpMethods.end() || std::find(httpVersions.begin(), httpVersions.end(), version) == httpVersions.end()) { 
                std::cout << "Received invalid HTTP packet" << std::endl;
                return;
            }

            // valid package
            std::cout << method + " " + path + " " + version << std::endl; // to del

            std::unordered_map<std::string, std::string> params;
            std::vector<std::string> splittedPath = splitString(path.c_str(), '?');
            if(splittedPath.size() == 2){
                path = splittedPath[0];
                params = parseQueryString(splittedPath[1]);
            }

            std::unordered_map<std::string, std::string> headers = this->getHeaders(lines);

            char* body = this->getBody(lines);

            // path handler
            if(method == "GET"){ 
                this->handleGetRequest(path, method, headers, params, body); 
                return;
            } 

            else if(method == "POST"){ 
                this->handlePostRequest(path, method, headers, params, body);
                return; 
            }

            else if(method == "PATCH"){ 
                this->handlePatchRequest(path, method, headers, params, body); 
                return;
            }

            else if(method == "DELETE"){ 
                this->handleDeleteRequest(path, method, headers, params, body); 
                return;
            }

            else { 
                this->handleUseRequest(path, method, headers, params, body);
                return;
            }
        }

        char* getBody(std::vector<std::string> lines){
            int bodyPos = -1;
            char* noBody;
            for (int i = 0; i < lines.size(); i++){
                if (lines[i] == "\r" ) {
                    bodyPos = i;
                    break;
                }
            }

            if(bodyPos == -1) {
                return noBody;
            }

            size_t totalLength = 0;
            for (int i = bodyPos + 1; i < lines.size(); i++) {
                totalLength += lines[i].length() + 1;
            }

            char* body = new char[totalLength];

            char* currentPos = body;
            for (int i = bodyPos + 1; i < lines.size(); i++) {
                std::strcpy(currentPos, lines[i].c_str());
                currentPos += lines[i].length() + 1;
            }

            return body;
        }

        std::unordered_map<std::string, std::string> getHeaders(std::vector<std::string> lines){
            std::unordered_map<std::string, std::string> headers;
            for (int i = 1; i<lines.size(); i++){
                if(lines[i].size() < 2) break;
                if(lines[i].find(":") == std::string::npos || splitString(lines[i].c_str(), ':').size() < 2){
                    std::cout << "Received invalid header: "+lines[i] << std::endl;
                    return {};
                }
                int separatorIndex = lines[i].find(":");
                std::string key = lines[i].substr(0, separatorIndex);
                std::string value = lines[i].substr(separatorIndex + 1);
                value.pop_back(); // deletes the final "\r"
                headers[key] = trimStart(value);
            }
            return headers;
        }

        std::vector<std::string> splitString(const char* str, char delimiter){
            std::vector<std::string> tokens;

            char* copy = new char[strlen(str) + 1];
            strcpy(copy, str);
            
            char* token = strtok(copy, &delimiter);
            
            while (token != NULL){
                tokens.push_back(token);
                token = strtok(NULL, &delimiter);
            }
            delete[] copy;
            
            return tokens;
        }

        std::unordered_map<std::string, std::string> parseQueryString(const std::string& queryString) {
            std::unordered_map<std::string, std::string> params;

            size_t startPos = 0;
            size_t endPos = queryString.find('&');

            while (endPos != std::string::npos) {
                std::string param = queryString.substr(startPos, endPos - startPos);

                size_t delimiterPos = param.find('=');
                if (delimiterPos != std::string::npos) {
                    std::string key = param.substr(0, delimiterPos);
                    std::string value = param.substr(delimiterPos + 1);
                    params[key] = value;
                }

                startPos = endPos + 1;
                endPos = queryString.find('&', startPos);
            }

            std::string lastParam = queryString.substr(startPos);
            size_t delimiterPos = lastParam.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = lastParam.substr(0, delimiterPos);
                std::string value = lastParam.substr(delimiterPos + 1);
                params[key] = value;
            }

            return params;
        }

        std::string trimStart(const std::string& str) {
            std::string trimmed = str;
            size_t startPos = 0;
            while (startPos < trimmed.length() && std::isspace(trimmed[startPos])) {
                ++startPos;
            }
            return trimmed.substr(startPos);
        }

        // testing purpose
        int printMap(std::unordered_map<std::string, std::string> map) {
            for (const auto& pair : map) {
                std::cout << pair.first << ": " << pair.second << std::endl;
            }

            return 0;
        }

        void handleGetRequest(std::string path, std::string method, std::unordered_map<std::string, std::string> headers, std::unordered_map<std::string, std::string> params, char* body){
            for (auto handler = getHandlers.begin(); handler != getHandlers.end(); handler++){
                if(handler->first == path){
                    std::cout << path << std::endl;
                    Request* req = new Request(path, method, headers, params, body);
                    Response* res = new Response(clientSocket);
                    for(const auto func : handler->second){
                        func(req, res);
                    }
                    delete req;
                    delete res;
                    return;
                }
            }

            std::string response = "Can't GET "+path;
            std::string httpResponse = "HTTP/1.1 " + std::to_string(400) +"\r\n";
            httpResponse += "Content-Type: text/plain\r\n";
            httpResponse += "Content-Length: " + std::to_string(response.length()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += response;
            write(clientSocket, httpResponse.c_str(), httpResponse.length());
            return;
        }

        void handlePostRequest(std::string path, std::string method, std::unordered_map<std::string, std::string> headers, std::unordered_map<std::string, std::string> params, char* body){
            for (auto handler = postHandlers.begin(); handler != postHandlers.end(); handler++){
                if(handler->first == path){
                    std::cout << path << std::endl;
                    Request* req = new Request(path, method, headers, params, body);
                    Response* res = new Response(clientSocket);
                    for(const auto func : handler->second){
                        func(req, res);
                    }
                    delete req;
                    delete res;
                    return;
                }
            }

            std::string response = "Can't post "+path;
            std::string httpResponse = "HTTP/1.1 " + std::to_string(400) +"\r\n";
            httpResponse += "Content-Type: text/plain\r\n";
            httpResponse += "Content-Length: " + std::to_string(response.length()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += response;
            write(clientSocket, httpResponse.c_str(), httpResponse.length());
            return;
        }

        void handlePatchRequest(std::string path, std::string method, std::unordered_map<std::string, std::string> headers, std::unordered_map<std::string, std::string> params, char* body){
            for (auto handler = patchHandlers.begin(); handler != patchHandlers.end(); handler++){
                if(handler->first == path){
                    std::cout << path << std::endl;
                    Request* req = new Request(path, method, headers, params, body);
                    Response* res = new Response(clientSocket);
                    for(const auto func : handler->second){
                        func(req, res);
                    }
                    delete req;
                    delete res;
                    return;
                }
            }

            std::string response = "Can't GET "+path;
            std::string httpResponse = "HTTP/1.1 " + std::to_string(400) +"\r\n";
            httpResponse += "Content-Type: text/plain\r\n";
            httpResponse += "Content-Length: " + std::to_string(response.length()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += response;
            write(clientSocket, httpResponse.c_str(), httpResponse.length());
            return;
        }

        void handleDeleteRequest(std::string path, std::string method, std::unordered_map<std::string, std::string> headers, std::unordered_map<std::string, std::string> params, char* body){
            for (auto handler = deleteHandlers.begin(); handler != deleteHandlers.end(); handler++){
                if(handler->first == path){
                    std::cout << path << std::endl;
                    Request* req = new Request(path, method, headers, params, body);
                    Response* res = new Response(clientSocket);
                    for(const auto func : handler->second){
                        func(req, res);
                    }
                    delete req;
                    delete res;
                    return;
                }
            }

            std::string response = "Can't GET "+path;
            std::string httpResponse = "HTTP/1.1 " + std::to_string(400) +"\r\n";
            httpResponse += "Content-Type: text/plain\r\n";
            httpResponse += "Content-Length: " + std::to_string(response.length()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += response;
            write(clientSocket, httpResponse.c_str(), httpResponse.length());
            return;
        }

        void handleUseRequest(std::string path, std::string method, std::unordered_map<std::string, std::string> headers, std::unordered_map<std::string, std::string> params, char* body){
            for (auto handler = useHandlers.begin(); handler != useHandlers.end(); handler++){
                if(handler->first == path){
                    std::cout << path << std::endl;
                    Request* req = new Request(path, method, headers, params, body);
                    Response* res = new Response(clientSocket);
                    for(const auto func : handler->second){
                        func(req, res);
                    }
                    delete req;
                    delete res;
                    return;
                }
            }

            std::string response = "Can't GET "+path;
            std::string httpResponse = "HTTP/1.1 " + std::to_string(400) +"\r\n";
            httpResponse += "Content-Type: text/plain\r\n";
            httpResponse += "Content-Length: " + std::to_string(response.length()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += response;
            write(clientSocket, httpResponse.c_str(), httpResponse.length());
            return;
        }

};

#endif
