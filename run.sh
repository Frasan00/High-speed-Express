#!/bin/bash

# install gnu compiler if not already installed
# sudo pacman -S gcc // installation for arch linux
# sudo apt-get update && sudo apt-get install -y build-essential on ubuntu

g++ ./src/main.cpp -o server.exe

./server.exe