#!/bin/bash

curl -X POST \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer eyjsdkljasjdlkasjfòklasfj" \
     -d '{"name": "Giggi", "age": "23", "email": "giggi@gmail.com"}' \
     "http://localhost:5000/user?query1=value1&query2=value2"