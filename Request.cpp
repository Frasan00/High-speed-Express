#ifndef REQUEST_H
#define REQUEST_H

#include <iostream>
#include <string>
#include <unordered_map>

class Request{

    public:
        std::string path;
        std::string method;
        std::unordered_map<std::string, std::string> headers;
        std::unordered_map<std::string, std::string> params;
        char* body;

        Request(std::string path, std::string method,  std::unordered_map<std::string, std::string> headers,  std::unordered_map<std::string, std::string> params, char* body){
            this->path = path;
            this->method = method;
            this->headers = headers;
            this->params = params;
            this->body = body;
        }
};

#endif