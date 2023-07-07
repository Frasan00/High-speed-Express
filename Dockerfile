FROM ubuntu:latest
WORKDIR /app
COPY ./src ./src
RUN apt-get update && \
    apt-get install -y build-essential && \
    rm -rf /var/lib/apt/lists/*
RUN g++ ./src/main.cpp -o server.exe
EXPOSE 5000
ENTRYPOINT [ "./server.exe" ]