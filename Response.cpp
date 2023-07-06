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
        std::string errorMessage = this->getErrorMessage(this->status);
        std::string httpResponse = "HTTP/1.1 " + std::to_string(status) + +" "+errorMessage+"\r\n";
        httpResponse += "Content-Type: text/plain\r\n";
        httpResponse += "Content-Length: " + std::to_string(response.length()) + "\r\n";
        httpResponse += "\r\n";
        httpResponse += response;

        write(clientSocket, httpResponse.c_str(), httpResponse.length());
    }

    std::string getErrorMessage(int status){
        std::string errorMessage = "";
        switch (status)
        {
            case 200:
                errorMessage = "OK";
                break;

            case 201:
                errorMessage = "Created";
                break;

            case 204:
                errorMessage = "No Content";
                break;

            case 400:
                errorMessage = "Bad Request";
                break;

            case 401:
                errorMessage = "Unauthorized";
                break;

            case 403:
                errorMessage = "Forbidden";
                break;

            case 404:
                errorMessage = "Not Found";
                break;

            case 500:
                errorMessage = "Internal Server Error";
                break;

            case 502:
                errorMessage = "Bad Gateway";
                break;

            case 503:
                errorMessage = "Service Unavailable";
                break;

            default:
                break;
        }

        return errorMessage;
    }
};

#endif
