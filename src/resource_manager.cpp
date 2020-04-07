/*
    This file holds the implementation of the class RessourceManager. Objects of this class own a vector of Resource Objects and helper methods to keep track of the available resources and their status.
*/
#include <iostream>
#include "resource.hpp"
#include "resource_manager.hpp"


ResourceManager::ResourceManager(std::vector<Resource*>&& resources)
{
    _resources = std::move(resources);
}


std::string ResourceManager::getResourceOverview()
{
    std::lock_guard<std::mutex> lock(_manager_mutex);

    std::string output;

    for(auto &resource : _resources)
    {
        output += resource->getName() + " : " + resource->getAvailability() + " : " + std::to_string(resource->getViews()) + " views\r\n";
    }

    return output;

}

std::vector<Resource*> ResourceManager::getResources()
{
    return _resources;
}