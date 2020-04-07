/*
    Header File for the class ResourceManager
*/
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include<string>
#include<vector>
#include<mutex>
#include "resource.hpp"

#ifndef RESOURCE_AVAILABILITY_H
#define RESOURCE_AVAILABILITY_H
#include "resource_availability.hpp"
#endif


class ResourceManager
{
    public:
    ResourceManager(std::vector<Resource*>&& resources);
    std::string getResourceOverview();
    std::vector<Resource*> getResources();
    
    private:
    std::vector<Resource*> _resources;
    std::mutex _manager_mutex;
};

#endif