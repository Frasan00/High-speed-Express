#ifndef RESPONSE_H
#define RESPONSE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <sys/socket.h>
#include <unistd.h>


class Response {
public:
    int status;
    int clientSocket;

    Response(int clientSocket) {
        this->clientSocket = clientSocket;
        this->status = 200;
    }

    Response setStatus(int status) {
        this->status = status;
        return *this;
    }

    void send(std::string response) {
        sendResponse(response);
    }


private:
    void sendResponse(const std::string& response) {
        std::string httpResponse = "HTTP/1.1 " + std::to_string(status) + " OK\r\n";
        httpResponse += "Content-Type: text/plain\r\n";
        httpResponse += "Content-Length: " + std::to_string(response.length()) + "\r\n";
        httpResponse += "\r\n";
        httpResponse += response;

        write(clientSocket, httpResponse.c_str(), httpResponse.length());
    }
};

#endif
