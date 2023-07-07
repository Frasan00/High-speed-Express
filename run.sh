#!/bin/bash

# install gnu compiler if not already installed
# sudo pacman -S gcc // installation for arch linux

g++ ./src/main.cpp -o server.exe

./server.exe