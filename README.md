# Express.cpp

- This is an attempt to re-create express from the famous node.js framework in cpp
- This project is intended to be a service that can handle http requests in an express-like way
- You can create your own handlers for each path with just few commands

### What this project does

- This project creates a simple TCP server on the desired port
- Then it listens for clients and creates a socket for each one that connects
- The server servers the clients based on the configured endpoint and handlers provided