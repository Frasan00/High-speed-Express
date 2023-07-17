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
#include "Router.cpp"

using namespace std;

/*
* Main process for a server socket: creation, binding to (ip, port), listen, connection
*/

vector<string> httpMethods = { "GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "TRACE", "CONNECT" };
vector<string> httpVersions = { "HTTP/1.0", "HTTP/1.1", "HTTP/2.0" };
typedef function<void(Request* req, Response* res)> FunctionType;

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
                cerr << "Failed to create server socket." << endl;
                return;
            }
            cout << "Socket created succesfully" << endl;
            int opt=1;
            socklen_t optlen=sizeof(opt);

            // binding the socket to (addr, port)
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_addr.s_addr = INADDR_ANY;
            serverAddress.sin_port  = htons(port);
            setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, optlen);
            int bindSocket = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
            if(bindSocket == -1){
                cerr << "Failed to bind the socket" << endl;
                close(serverSocket);
                return;
            }

            int listenSocket = listen(serverSocket, max_connections);
            if(listenSocket == -1){
                cerr << "Failed to listen for incoming connections" << endl;
                close(serverSocket);
                return;
            }

            // passive listen for incoming connections
            while(true){
                clientAddressLength = sizeof(clientAddress);
                clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);

                if (clientSocket == -1) {
                    cerr << "Failed to accept connection." << endl;
                    close(serverSocket);
                    return;
                }

                // client connected
                cout << "Client connected, socket: "+clientSocket << endl;

                // handle incoming message
                const int bufferSize = 1024;
                char buffer[bufferSize];
                memset(buffer, 0, bufferSize);

                ssize_t bytesRead = recv(clientSocket, buffer, bufferSize - 1, 0);
                if (bytesRead == -1) { cerr << "Failed to read data from client." << endl; } 
                else if (bytesRead == 0) { cout << "Client disconnected. Socket: " << clientSocket << endl; } 
                else {
                    // Data received successfully
                    cout << "Data received from socket: "+clientSocket << endl;
                    this->handleData(serverSocket, clientSocket, buffer);
                }
                
                // Close the client socket
                /*close(clientSocket);
                cout << "Client disconnected, socket: " << clientSocket << endl;*/
            }
        }

        void route(string path, Router router) {
            const vector<unordered_map<string, vector<FunctionType>>>& routes = router.getRouteInfo();
            
            for (int i = 0; i < routes.size(); i++) {
                const unordered_map<string, vector<FunctionType>>& route = routes[i];
                
                for (const auto& entry : route) {
                    const string& routeKey = path+entry.first;
                    const vector<FunctionType>& routeHandlers = entry.second;

                    switch (i) {
                        case 0:
                            if (getHandlers.count(routeKey) > 0) { break; }
                            getHandlers[routeKey] = routeHandlers;
                            break;

                        case 1:
                            if (postHandlers.count(routeKey) > 0) { break; }
                            postHandlers[routeKey] = routeHandlers;
                            break;

                        case 2:
                            if (patchHandlers.count(routeKey) > 0) { break; }
                            patchHandlers[routeKey] = routeHandlers;
                            break;

                        case 3:
                            if (deleteHandlers.count(routeKey) > 0) { break; }
                            deleteHandlers[routeKey] = routeHandlers;
                            break;

                        default:
                            if (useHandlers.count(routeKey) > 0) { break; }
                            useHandlers[routeKey] = routeHandlers;
                            break;
                    }
                }
            }
        }

        // simple handlers
        void use(string path, FunctionType handler){
            if (useHandlers.count(path) > 0) {
                return;
            }
            useHandlers[path].push_back(handler);
        }

        void get(string path, FunctionType handler){
            if (getHandlers.count(path) > 0) {
                return;
            }
            getHandlers[path].push_back(handler);
        }

        void post(string path, FunctionType handler){
            if (postHandlers.count(path) > 0) {
                return;
            }
            postHandlers[path].push_back(handler);
        }

        void patch(string path, FunctionType handler){
            if (patchHandlers.count(path) > 0) {
                return;
            }
            patchHandlers[path].push_back(handler);
        }

        void del(string path, FunctionType handler){
            if (deleteHandlers.count(path) > 0) {
                return;
            }
            deleteHandlers[path].push_back(handler);
        }

        // handlers with middlewares
        void use(string path, vector<FunctionType> middlewares, FunctionType handler){
            if (useHandlers.count(path) > 0) {
                return;
            }
            for(const auto& middleware : middlewares) useHandlers[path].push_back(middleware);
            useHandlers[path].push_back(handler);
        }

        void get(string path, vector<FunctionType> middlewares, FunctionType handler){
            if (getHandlers.count(path) > 0) {
                return;
            }
            for(const auto& middleware : middlewares) getHandlers[path].push_back(middleware);
            getHandlers[path].push_back(handler);
        }

        void post(string path, vector<FunctionType> middlewares, FunctionType handler){
            if (postHandlers.count(path) > 0) {
                return;
            }
            for(const auto& middleware : middlewares) postHandlers[path].push_back(middleware);
            postHandlers[path].push_back(handler);
        }

        void patch(string path, vector<FunctionType> middlewares, FunctionType handler){
            if (patchHandlers.count(path) > 0) {
                return;
            }
            for(const auto& middleware : middlewares) patchHandlers[path].push_back(middleware);
            patchHandlers[path].push_back(handler);
        }

        void del(string path, vector<FunctionType> middlewares, FunctionType handler){
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
        unordered_map<string, vector<FunctionType>> useHandlers, getHandlers, postHandlers, patchHandlers, deleteHandlers;

        void handleData(int serverSocket, int clientSocket, char* buffer){
            vector<string> lines = splitString(buffer, '\n');
            string firstLine = lines[0];
            vector<string> splittedFirstLine = splitString(firstLine.c_str(), ' ');

            // First packet check
            if (strlen(buffer) == 0 || lines.size() < 1 || splittedFirstLine.size() < 2) { 
                cout << "Received invalid HTTP packet" << endl;
                return;
            }

            string method = splittedFirstLine[0];
            string path = splittedFirstLine[1];
            string version = splittedFirstLine[2];
            version.pop_back();

            // second packet check
            if (find(httpMethods.begin(), httpMethods.end(), method) == httpMethods.end() || find(httpVersions.begin(), httpVersions.end(), version) == httpVersions.end()) { 
                cout << "Received invalid HTTP packet" << endl;
                return;
            }

            // valid package
            cout << method + " " + path + " " + version << endl; // to del

            unordered_map<string, string> params;
            vector<string> splittedPath = splitString(path.c_str(), '?');
            if(splittedPath.size() == 2){
                path = splittedPath[0];
                params = parseQueryString(splittedPath[1]);
            }

            unordered_map<string, string> headers = getHeaders(lines);

            char* body = getBody(lines);

            // path handler
            if(method == "GET"){ 
                handleGetRequest(path, method, headers, params, body); 
                return;
            } 

            else if(method == "POST"){ 
                handlePostRequest(path, method, headers, params, body);
                return; 
            }

            else if(method == "PATCH"){ 
                handlePatchRequest(path, method, headers, params, body); 
                return;
            }

            else if(method == "DELETE"){ 
                handleDeleteRequest(path, method, headers, params, body); 
                return;
            }

            else { 
                handleUseRequest(path, method, headers, params, body);
                return;
            }
        }

        char* getBody(vector<string> lines){
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
                strcpy(currentPos, lines[i].c_str());
                currentPos += lines[i].length() + 1;
            }

            return body;
        }

        unordered_map<string, string> getHeaders(vector<string> lines){
            unordered_map<string, string> headers;
            for (int i = 1; i<lines.size(); i++){
                if(lines[i].size() < 2) break;
                if(lines[i].find(":") == string::npos || splitString(lines[i].c_str(), ':').size() < 2){
                    cout << "Received invalid header: "+lines[i] << endl;
                    return {};
                }
                int separatorIndex = lines[i].find(":");
                string key = lines[i].substr(0, separatorIndex);
                string value = lines[i].substr(separatorIndex + 1);
                value.pop_back(); // deletes the final "\r"
                headers[key] = trimStart(value);
            }
            return headers;
        }

        vector<string> splitString(const char* str, char delimiter){
            vector<string> tokens;

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

        unordered_map<string, string> parseQueryString(const string& queryString) {
            unordered_map<string, string> params;

            size_t startPos = 0;
            size_t endPos = queryString.find('&');

            while (endPos != string::npos) {
                string param = queryString.substr(startPos, endPos - startPos);

                size_t delimiterPos = param.find('=');
                if (delimiterPos != string::npos) {
                    string key = param.substr(0, delimiterPos);
                    string value = param.substr(delimiterPos + 1);
                    params[key] = value;
                }

                startPos = endPos + 1;
                endPos = queryString.find('&', startPos);
            }

            string lastParam = queryString.substr(startPos);
            size_t delimiterPos = lastParam.find('=');
            if (delimiterPos != string::npos) {
                string key = lastParam.substr(0, delimiterPos);
                string value = lastParam.substr(delimiterPos + 1);
                params[key] = value;
            }

            return params;
        }

        string trimStart(const string& str) {
            string trimmed = str;
            size_t startPos = 0;
            while (startPos < trimmed.length() && isspace(trimmed[startPos])) {
                ++startPos;
            }
            return trimmed.substr(startPos);
        }

        void handleGetRequest(string path, string method, unordered_map<string, string> headers, unordered_map<string, string> params, char* body){
            for (auto handler = getHandlers.begin(); handler != getHandlers.end(); handler++){
                if(handler->first == path){
                    cout << path << endl;
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

            string response = "Can't GET "+path;
            string httpResponse = "HTTP/1.1 " + to_string(400) +"\r\n";
            httpResponse += "Content-Type: text/plain\r\n";
            httpResponse += "Content-Length: " + to_string(response.length()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += response;
            write(clientSocket, httpResponse.c_str(), httpResponse.length());
            return;
        }

        void handlePostRequest(string path, string method, unordered_map<string, string> headers, unordered_map<string, string> params, char* body){
            for (auto handler = postHandlers.begin(); handler != postHandlers.end(); handler++){
                if(handler->first == path){
                    cout << path << endl;
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

            string response = "Can't post "+path;
            string httpResponse = "HTTP/1.1 " + to_string(400) +"\r\n";
            httpResponse += "Content-Type: text/plain\r\n";
            httpResponse += "Content-Length: " + to_string(response.length()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += response;
            write(clientSocket, httpResponse.c_str(), httpResponse.length());
            return;
        }

        void handlePatchRequest(string path, string method, unordered_map<string, string> headers, unordered_map<string, string> params, char* body){
            for (auto handler = patchHandlers.begin(); handler != patchHandlers.end(); handler++){
                if(handler->first == path){
                    cout << path << endl;
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

            string response = "Can't GET "+path;
            string httpResponse = "HTTP/1.1 " + to_string(400) +"\r\n";
            httpResponse += "Content-Type: text/plain\r\n";
            httpResponse += "Content-Length: " + to_string(response.length()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += response;
            write(clientSocket, httpResponse.c_str(), httpResponse.length());
            return;
        }

        void handleDeleteRequest(string path, string method, unordered_map<string, string> headers, unordered_map<string, string> params, char* body){
            for (auto handler = deleteHandlers.begin(); handler != deleteHandlers.end(); handler++){
                if(handler->first == path){
                    cout << path << endl;
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

            string response = "Can't GET "+path;
            string httpResponse = "HTTP/1.1 " + to_string(400) +"\r\n";
            httpResponse += "Content-Type: text/plain\r\n";
            httpResponse += "Content-Length: " + to_string(response.length()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += response;
            write(clientSocket, httpResponse.c_str(), httpResponse.length());
            return;
        }

        void handleUseRequest(string path, string method, unordered_map<string, string> headers, unordered_map<string, string> params, char* body){
            for (auto handler = useHandlers.begin(); handler != useHandlers.end(); handler++){
                if(handler->first == path){
                    cout << path << endl;
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

            string response = "Can't GET "+path;
            string httpResponse = "HTTP/1.1 " + to_string(400) +"\r\n";
            httpResponse += "Content-Type: text/plain\r\n";
            httpResponse += "Content-Length: " + to_string(response.length()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += response;
            write(clientSocket, httpResponse.c_str(), httpResponse.length());
            return;
        }

};

#endif
