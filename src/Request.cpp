#ifndef REQUEST_H
#define REQUEST_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

class Request{

    public:
        string path;
        string method;
        unordered_map<string, string> headers;
        unordered_map<string, string> params;
        unordered_map<string, string>  body;

        Request(string path, string method,  unordered_map<string, string> headers,  unordered_map<string, string> params, string body){
            this->path = path;
            this->method = method;
            this->headers = headers;
            this->params = params;
            this->body = extractBody(body);;
        }


        string getHeader(string headerName){
            for (const auto header : this->headers){
                if(header.first == headerName){
                    return header.second;
                }
            }
            return "";
        }

        string getQueryParam(string queryParamName){
            for (const auto queryParam : this->params){
                if(queryParam.first == queryParamName){
                    return queryParam.second;
                }
            }
            return "";         
        }

        string getBodyParam(string bodyParamName) {
            for (const auto bodyParam : this->body){
                string key = bodyParam.first;
                string value = bodyParam.second;
                key.erase(remove(key.begin(), key.end(), '\"'), key.end());
                value.erase(remove(value.begin(), value.end(), '\"'), value.end());
                value.erase(remove(value.begin(), value.end(), '\r'), value.end());
                value.erase(remove(value.begin(), value.end(), '\n'), value.end());

                if(key == bodyParamName){
                    string out =value;
                    return out;
                }
            }
            return "";
        }

    private:
        unordered_map<string, string> extractBody(const string& body) {
            unordered_map<string, string> hashMap;
            string trimmedBody = body.substr(1, body.length() - 2);

            size_t startPos = 0;
            size_t endPos = trimmedBody.find(',');
            
            while (endPos != string::npos) {
                string pair = trimmedBody.substr(startPos, endPos - startPos);

                size_t colonPos = pair.find(':');
                if (colonPos != string::npos) {
                    string key = pair.substr(1, colonPos - 2);
                    string value = pair.substr(colonPos + 2, pair.length() - colonPos - 3);

                    hashMap[key] = value;
                }

                startPos = endPos + 1;
                endPos = trimmedBody.find(',', startPos);
            }
            string lastPair = trimmedBody.substr(startPos);
            size_t lastColonPos = lastPair.find(':');
            if (lastColonPos != string::npos) {
                string key = lastPair.substr(1, lastColonPos - 2);
                string value = lastPair.substr(lastColonPos + 2, lastPair.length() - lastColonPos - 3);

                hashMap[key] = value;
            }

            return hashMap;
        }
        
};

#endif