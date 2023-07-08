#ifndef REQUEST_H
#define REQUEST_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

class Request{

    public:
        std::string path;
        std::string method;
        std::unordered_map<std::string, std::string> headers;
        std::unordered_map<std::string, std::string> params;
        std::unordered_map<std::string, std::string> body;

        Request(std::string path, std::string method,  std::unordered_map<std::string, std::string> headers,  std::unordered_map<std::string, std::string> params, char* body){
            this->path = path;
            this->method = method;
            this->headers = headers;
            this->params = params;
    
        }


        std::string getHeader(std::string headerName){
            for (const auto header : this->headers){
                if(header.first == headerName){
                    return header.second;
                }
            }
            return "";
        }

        std::string getQueryParam(std::string queryParamName){
            for (const auto queryParam : this->params){
                if(queryParam.first == queryParamName){
                    return queryParam.second;
                }
            }
            return "";         
        }

        std::string getBodyParam(std::string bodyParamName) {
            for (const auto& bodyParam : body) {
                if (bodyParam.first == bodyParamName) {
                    return bodyParam.second;
                }
            }
            return "";
        }

    private:
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
        
};

#endif