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
        std::unordered_map<std::string, std::string>  body;

        Request(std::string path, std::string method,  std::unordered_map<std::string, std::string> headers,  std::unordered_map<std::string, std::string> params, std::string body){
            this->path = path;
            this->method = method;
            this->headers = headers;
            this->params = params;
            this->body = extractBody(body);;
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
            for (const auto bodyParam : this->body){
                std::string key = bodyParam.first;
                std::string value = bodyParam.second;
                key.erase(std::remove(key.begin(), key.end(), '\"'), key.end());
                value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
                value.erase(std::remove(value.begin(), value.end(), '\r'), value.end());
                value.erase(std::remove(value.begin(), value.end(), '\n'), value.end());

                if(key == bodyParamName){
                    std::string out =value;
                    return out;
                }
            }
            return "";
        }

    private:
        std::unordered_map<std::string, std::string> extractBody(const std::string& body) {
            std::unordered_map<std::string, std::string> hashMap;
            std::string trimmedBody = body.substr(1, body.length() - 2);

            size_t startPos = 0;
            size_t endPos = trimmedBody.find(',');
            
            while (endPos != std::string::npos) {
                std::string pair = trimmedBody.substr(startPos, endPos - startPos);

                size_t colonPos = pair.find(':');
                if (colonPos != std::string::npos) {
                    std::string key = pair.substr(1, colonPos - 2);
                    std::string value = pair.substr(colonPos + 2, pair.length() - colonPos - 3);

                    hashMap[key] = value;
                }

                startPos = endPos + 1;
                endPos = trimmedBody.find(',', startPos);
            }
            std::string lastPair = trimmedBody.substr(startPos);
            size_t lastColonPos = lastPair.find(':');
            if (lastColonPos != std::string::npos) {
                std::string key = lastPair.substr(1, lastColonPos - 2);
                std::string value = lastPair.substr(lastColonPos + 2, lastPair.length() - lastColonPos - 3);

                hashMap[key] = value;
            }

            return hashMap;
        }
        
};

#endif