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
    int serverSocket;

    Response(int serverSocket) {
        this->serverSocket = serverSocket;
        this->status = 200;
    }

    Response setStatus(int status) {
        this->status = status;
        return *this;
    }

    void send(std::string response) {
        sendResponse(response);
    }

    void send(char* response) {
        sendResponse(response);
    }

    void send(int response) {
        std::string strResponse = std::to_string(response);
        sendResponse(strResponse);
    }

    void send(std::unordered_map<std::string, std::string> response) {
        std::string strResponse = formatMapResponse(response);
        sendResponse(strResponse);
    }

    void send(std::unordered_map<std::string, int> response) {
        std::string strResponse = formatMapResponse(response);
        sendResponse(strResponse);
    }

    void send(std::unordered_map<int, std::string> response) {
        std::string strResponse = formatMapResponse(response);
        sendResponse(strResponse);
    }

    void send(std::unordered_map<std::string, bool> response) {
        std::string strResponse = formatMapResponse(response);
        sendResponse(strResponse);
    }

    void send(std::unordered_map<bool, std::string> response) {
        std::string strResponse = formatMapResponse(response);
        sendResponse(strResponse);
    }

    void send(std::unordered_map<int, bool> response) {
        std::string strResponse = formatMapResponse(response);
        sendResponse(strResponse);
    }

    void send(std::unordered_map<bool, int> response) {
        std::string strResponse = formatMapResponse(response);
        sendResponse(strResponse);
    }

private:
    void sendResponse(const std::string& response) {
        std::string httpResponse = "HTTP/1.1 " + std::to_string(status) + " OK\r\n";
        httpResponse += "Content-Type: text/plain\r\n";
        httpResponse += "Content-Length: " + std::to_string(response.length()) + "\r\n";
        httpResponse += "\r\n";
        httpResponse += response;

        write(serverSocket, httpResponse.c_str(), httpResponse.length());
    }

    template<typename MapType>
    std::string formatMapResponse(const MapType& response) {
        std::string strResponse;
        for (const auto& pair : response) {
            strResponse += pair.first + ": " + std::to_string(pair.second) + "\n";
        }
        return strResponse;
    }
};

#endif
