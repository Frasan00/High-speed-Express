#include <string>
#include "Server.cpp"
#include "Request.cpp"
#include "Response.cpp"

// User Class
class User{
    public:
        int id;
        std::string name;
        std::string email;
        std::string age;
};

// simple db
std::vector<User> db = {};

void getAllUsers(Request* req, Response* res) {
    if(db.size() == 0) {
        res->setStatus(404).send("There are no users in the database");
        return;
    }

    std::string response;
    for (const auto& user : db) {
        response += "ID: " + std::to_string(user.id) + "\n";
        response += "Name: " + user.name + "\n";
        response += "Email: " + user.email + "\n";
        response += "Age: " + user.age + "\n";
        response += "\n";
    }

    res->setStatus(200).send(response);
}

void getUser(Request* req, Response* res) {
    if(db.size() == 0) {
        res->setStatus(404).send("There are no users in the database");
        return;
    }
    int userId = std::stoi(req->getQueryParam("id"));

    std::string response = "";
    for (const User& user : db) {
        if (user.id == userId) {
            response += "ID: " + std::to_string(user.id) + "\n";
            response += "Name: " + user.name + "\n";
            response += "Email: " + user.email + "\n";
            response += "Age: " + user.age + "\n";
            response += "\n";
            break;
        }
    }
    if(response == "") return res->setStatus(404).send("User not found");
    else res->setStatus(404).send(response);
}

void createUser(Request* req, Response* res) {
    std::string name = req->getBodyParam("name");
    std::string email = req->getBodyParam("email"); 
    std::string age = req->getBodyParam("age");

    User newUser;
    newUser.id = db.size() > 0 ? db[db.size() - 1].id + 1 : 1;
    newUser.name = name;
    newUser.email = email;
    newUser.age = age;

    db.push_back(newUser);

    res->setStatus(200).send("User created successfully");
}

void deleteUser(Request* req, Response* res) {
    if (db.empty()) {
        res->setStatus(404).send("There are no users in the database");
        return;
    }

    int userId = std::stoi(req->getQueryParam("id"));

    auto user = std::find_if(db.begin(), db.end(), [userId](const User& user) {
        return user.id == userId;
    });

    if (user != db.end()) {
        db.erase(user);
        res->setStatus(200).send("User deleted successfully");
    } else {
        res->setStatus(404).send("User not found");
    }
}

int main(){
    /* Simple User REST api */

    // server creation (port, max connections)
    Server* server = new Server(5000, 10);


    server->get("/users", getAllUsers);
    server->get("/user", getUser);
    server->post("/user", createUser);
    server->del("/user", deleteUser);
    server->start();

    delete server;

    return 0;
}