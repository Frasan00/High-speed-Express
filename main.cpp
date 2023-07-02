#include <string>
#include "Server.cpp"

int main(){
    Server* server = new Server(5000, 10);
    server->start();

    delete server;

    return 0;
}