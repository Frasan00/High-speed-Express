#!/bin/bash

docker compose down
docker system prune -f
docker rmi $(docker images -a -q)