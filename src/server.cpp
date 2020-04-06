#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <thread>
#include <vector>
#include <memory>

int clientCounter{0};

void clientHandler(sockaddr_in&& client, int&& clientSocket)
{
    clientCounter ++;
    std::cout << "Started client handler for client " << clientCounter << std::endl;

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    if(getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        std::cout << host << "connected on port " << service << std::endl;
    }

    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << "connected on port" << ntohs(client.sin_port) << std::endl;
    }

    char buf[4096];

    while(true)
    {
        memset(buf, 0, 4096);

        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if(bytesReceived == -1)
        {
            std::cerr << "Error in recv() Quitting!" << std::endl;
            break;

        }

        if(bytesReceived == 0)
        {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        std::cout << std::string(buf, 0, bytesReceived) << std::endl;

        send(clientSocket, buf, bytesReceived + 1, 0);

    }



}

// TODO define vector of resources which have be locked during modification
int main()
{
    
    std::vector<std::thread> client_threads;

    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1)
    {
        std::cerr << "Can not create socket";
        return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    //Max 10 client can be served at once
    listen(listening, 10);

    while(true)
    {   
        sockaddr_in client;
        socklen_t clientSize = sizeof(client);

        int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
        client_threads.emplace_back(std::thread(clientHandler, std::move(client), std::move(clientSocket))); 
    }

    for(auto &thread : client_threads)
        {
            thread.join();
        }
   
    

    //close(clientSocket);

    return 0;
    
}




