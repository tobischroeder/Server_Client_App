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
#include <map>
#include <algorithm>
#include <condition_variable>
#include <chrono>
#include "resource.hpp"
#include "resource_manager.hpp"

class Resource;
class ResourceManager;

int clientCounter{0};
std::mutex client_handler_mutex;
std::condition_variable con_var;
std::vector<Resource*> resources = {new Resource("doc1"), new Resource("doc2"), new Resource("doc3")};
ResourceManager* resource_manager = new ResourceManager(std::move(resources));
void incrementClientCounter()
{
    //std::lock_guard<std::mutex> client_handler_lock(client_handler_mutex);
    clientCounter += 1;
    std::cout << "Started client handler for client " << clientCounter << std::endl;

}


void clientHandler(sockaddr_in&& client, int&& clientSocket)
{
    clientCounter += 1;
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
        std::cout << "entered while" << std::endl;
        std::string askClient = "Hello Client " + std::to_string(clientCounter) + " which resource would you like to get allocated?\r\n";
        std::string resourceOverview = resource_manager->getResourceOverview();
        send(clientSocket, (askClient + resourceOverview).c_str(), (askClient + resourceOverview).size() + 1, 0);
        
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if(bytesReceived == -1)
        {
            std::cout << "Error in recv() Quitting!" << std::endl;
            break;

        }

        if(bytesReceived == 0)
        {
            std::cout << "Client disconnected" << std::endl;
            break;
        }


        std::string clientInput = std::string(buf, bytesReceived);

        bool resource_found = false;
        bool successfull_locked = false;

        std::string response;
        clientInput.erase(std::find(clientInput.begin(), clientInput.end(), '\0'), clientInput.end());

        for(auto &resource : resource_manager->getResources())
        {
            if((*resource).getName() == clientInput && !resource_found)
            {
                resource_found = true;

                if((*resource).getAvailability() == "locked")
                {
                    response = "Resource is currently not available!\r\n\n";
                    
                }

                else
                {
                    (*resource).toggleAvailability();
                    successfull_locked = true;
                    response = "Resource " +  (*resource).getName() + " allocated. Please type unlock to give back ownership\r\n";
                } 
            }


        }

        if(!resource_found)
        {
            response =  "The requested resource is not available.\r\n";
        }


        send(clientSocket, response.c_str(), response.size() + 1, 0);

        if(successfull_locked)
        {
            bytesReceived = recv(clientSocket, buf, 4096, 0);

            if(bytesReceived == -1)
            {
                std::cout << "Error in recv() Quitting!" << std::endl;
                break;

            }

            if(bytesReceived == 0)
            {
                std::cout << "Client disconnected" << std::endl;
                break;
            }


            std::string unlock = std::string(buf, bytesReceived);
            unlock.erase(std::find(unlock.begin(), unlock.end(), '\0'), unlock.end());

            bool unlock_success = false;

            if(unlock == "unlock")
            {
                for(auto &resource : resource_manager->getResources())
                {
                    if( (*resource).getName() == clientInput)
                    {
                        (*resource).toggleAvailability();
                        unlock_success = true;
                        std::cout << "Resource " <<  (*resource).getName() << " deallocated" << std::endl; 
                    }


                }

                if(!unlock_success)
                {
                    std::cout << "Error: Unlock not possible." << std::endl;
                }

            }
        }

        else
        {
            continue;
        }
        
    }
}

int main()
{
    std::vector<std::thread> client_threads;
    
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1)
    {
        std::cout << "Can not create socket";
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

    return 0;
    
}




