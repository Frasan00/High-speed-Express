#include <string>
#include "Server.cpp"
#include "Request.cpp"
#include "Response.cpp"

void handler(Request* req, Response* res){
    std::cout << "Hello!" << "";
}

int main(){
    Server* server = new Server(5000, 10);
    server->addHandler("/", handler);
    server->start();

    delete server;

    return 0;
}