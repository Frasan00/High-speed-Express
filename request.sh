#!/bin/bash

curl -X GET \
     -H "Content-Type: application/json" \
     -H "Authorization: Bearer eyjsdkljasjdlkasjfòklasfj" \
     -d '{"param1": "value1", "param2": "value2", "param3": "value3", "param4": "value4"}' \
     "http://localhost:5000/?query1=value1&query2=value2"