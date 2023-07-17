#!/bin/bash

function perform_curl_test() {
    endpoint=$1
    request_type=$2
    body=$3

    echo "Testing $request_type $endpoint"
    echo "-------------------------------------"

    curl_output=$(curl -w "\n\nTime Total: %{time_total}s\nTime Connect: %{time_connect}s\nTime AppConnect: %{time_appconnect}s\nTime Redirect: %{time_redirect}s\nTime Pretransfer: %{time_pretransfer}s\nTime StartTransfer: %{time_starttransfer}s\n\n" -o /dev/null -s -X $request_type -H "Content-Type: application/json" -d "$body" "http://localhost:5000/$endpoint")

    echo "$curl_output"
    echo "-------------------------------------"
    echo
}

# Perform curl for each endpoint with timing
perform_curl_test "users" "GET"
perform_curl_test "user?id=1" "GET"
perform_curl_test "user" "POST" '{"name": "John Doe", "email": "johndoe@example.com", "age": "30"}'
perform_curl_test "user?id=1" "DELETE"

rm -f response.txt