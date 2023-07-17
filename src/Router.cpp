#ifndef ROUTER_H
#define ROUTER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include "Request.cpp"
#include "Response.cpp"

using namespace std;

typedef function<void(Request* req, Response* res)> FunctionType;

class Router {

    public:
        void init(string basePath){ this->basePath = basePath; }

        string getPath(){ return this->basePath; }

        void use(string path, FunctionType handler){
            if (useHandlers.count(path) > 0) {
                return;
            }
            useHandlers[this->basePath+path].push_back(handler);
        }

        void get(string path, FunctionType handler){
            if (getHandlers.count(path) > 0) {
                return;
            }
            getHandlers[this->basePath+path].push_back(handler);
        }

        void post(string path, FunctionType handler){
            if (postHandlers.count(path) > 0) {
                return;
            }
            postHandlers[this->basePath+path].push_back(handler);
        }

        void patch(string path, FunctionType handler){
            if (patchHandlers.count(path) > 0) {
                return;
            }
            patchHandlers[this->basePath+path].push_back(handler);
        }

        void del(string path, FunctionType handler){
            if (deleteHandlers.count(path) > 0) {
                return;
            }
            deleteHandlers[this->basePath+path].push_back(handler);
        }

        // handlers with middlewares
        void use(string path, vector<FunctionType> middlewares, FunctionType handler){
            if (useHandlers.count(path) > 0) {
                return;
            }
            for(const auto& middleware : middlewares) useHandlers[this->basePath+path].push_back(middleware);
            useHandlers[this->basePath+path].push_back(handler);
        }

        void get(string path, vector<FunctionType> middlewares, FunctionType handler){
            if (getHandlers.count(path) > 0) {
                return;
            }
            for(const auto& middleware : middlewares) getHandlers[this->basePath+path].push_back(middleware);
            getHandlers[this->basePath+path].push_back(handler);
        }

        void post(string path, vector<FunctionType> middlewares, FunctionType handler){
            if (postHandlers.count(path) > 0) {
                return;
            }
            for(const auto& middleware : middlewares) postHandlers[this->basePath+path].push_back(middleware);
            postHandlers[this->basePath+path].push_back(handler);
        }

        void patch(string path, vector<FunctionType> middlewares, FunctionType handler){
            if (patchHandlers.count(path) > 0) {
                return;
            }
            for(const auto& middleware : middlewares) patchHandlers[this->basePath+path].push_back(middleware);
            patchHandlers[this->basePath+path].push_back(handler);
        }

        void del(string path, vector<FunctionType> middlewares, FunctionType handler){
            if (deleteHandlers.count(path) > 0) {
                return;
            }
            for(const auto& middleware : middlewares) deleteHandlers[this->basePath+path].push_back(middleware);
            deleteHandlers[this->basePath+path].push_back(handler);
        }

        vector<unordered_map<string, vector<FunctionType>>> getRouteInfo(){
            return { this->getHandlers, this->postHandlers, this->patchHandlers, this->deleteHandlers, this->useHandlers };
        }
    
    private:
        string basePath;
        unordered_map<string, vector<FunctionType>> useHandlers, getHandlers, postHandlers, patchHandlers, deleteHandlers;
};

#endif