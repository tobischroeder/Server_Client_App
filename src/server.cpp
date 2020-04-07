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


//Global Variables
int clientCounter{0};
std::mutex client_handler_mutex;
std::condition_variable con_var;
std::vector<Resource*> resources = {new Resource("document1"), new Resource("document2"), new Resource("document3")};
std::unique_ptr<ResourceManager> resource_manager (new ResourceManager(std::move(resources)));

//global variable client counter is incremented whenever the server has accepted a new client connection
void incrementClientCounter()
{
    std::lock_guard<std::mutex> client_handler_lock(client_handler_mutex);
    clientCounter += 1;
    std::cout << "Started client handler for client " << clientCounter << std::endl;

}

//Method is called from the client thread, handles the communication between server and client

void clientHandler(sockaddr_in&& client, int&& clientSocket)
{
    //Establish server client connection
    incrementClientCounter();
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
    
    //Loop which handles the communication between server and client after connection was established

    while(true)
    {
        //The communication is started from server side
        memset(buf, 0, 4096);
        std::string askClient = "Hello Client " + std::to_string(clientCounter) + " which resource would you like to get allocated?\r\n";
        std::string resourceOverview = resource_manager->getResourceOverview();
        send(clientSocket, (askClient + resourceOverview).c_str(), (askClient + resourceOverview).size() + 1, 0);

        //Wait for response from client, which resource shall be allocated?
        
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

        //Boolean helper to keep track of allocation process
        bool resource_found = false;
        bool successfull_locked = false;

        std::string response;

        //Erase ending null of the string to reach string comparibility

        clientInput.erase(std::find(clientInput.begin(), clientInput.end(), '\0'), clientInput.end());

        //Iterate over the resource vector held by the resource manager 
        for(auto &resource : resource_manager->getResources())
        {
            //check if demanded resource is equal to vector resource in current iteration
            if((*resource).getName() == clientInput && !resource_found)
            {
                resource_found = true;
                //check if the demanded resource is already locked
                if((*resource).getAvailability() == "locked")
                {
                    response = "Resource is currently not available!\r\n\n";
                    
                }
                //if resource is free - allocate 
                else
                {
                    (*resource).toggleAvailability();
                    successfull_locked = true;
                    response = "Resource " +  (*resource).getName() + " allocated. Please type unlock to give back ownership\r\n";
                } 
            }


        }

        //Check if the demanded resource was found

        if(!resource_found)
        {
            response =  "The requested resource is not available.\r\n";
        }

        //Send the search and allocation result back to the client
        send(clientSocket, response.c_str(), response.size() + 1, 0);

        //If the demanded resource was successfully locked -> wait for client to unlock
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

            //check clients unlock response

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
                //catch the case where clients response is different than "unlock"
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
        //create socket and thread, add the thread to the vector of threads, call clientHandler from thread and move client and clientSocket ownership
        int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
        client_threads.emplace_back(std::thread(clientHandler, std::move(client), std::move(clientSocket))); 
    }

    for(auto &thread : client_threads)
        {
            thread.join();
        }

    return 0;
    
}




