#!/bin/bash

(
echo -e "GET /?query1=value1&query2=value2 HTTP/1.1\r\nHost: localhost:5000\r\nContent-Type: application/json\r\nAuthorization: Bearer eyjsdkljasjdlkasjfòklasfj\r\nContent-Length: 80\r\n\r\n{\"param1\": \"value1\", \"param2\": \"value2\", \"param3\": \"value3\", \"param4\": \"value4\"}"
) | nc localhost 5000