/*
    This file holds the implementation of the class Resource. Objects of this class represent for example a document which can only be controlled by one client-thread at any time.
    It holds a name, a view counter and the availibility information
*/

#include "resource.hpp"
#include<iostream>

Resource::Resource(std::string name)
{
    _name = name;
    _views = 0;
    _availability = Availability::unlocked;
}

int Resource::getViews() const
{
    return _views;
}

std::string Resource::getAvailability() const
{
    if(_availability == Availability::locked)
    {
        return "locked";
    }

    else
    {
        return "unlocked";
    }

}

void Resource::incrementViews() 
{
    _views += 1;
}

std::string Resource::getName() const
{
    return _name;
}

void Resource::setName(std::string name)
{
    _name = name;
}

void Resource::toggleAvailability()
{
    std::lock_guard<std::mutex> lock(_resource_mutex);

    if(_availability == Availability::unlocked)
    {
        _availability = Availability::locked;
        incrementViews();
        std::cout << "Resource " << _name << " was locked.\r\n";
    }

    else if(_availability == Availability::locked)
    {
        _availability = Availability::unlocked;
        std::cout << "Resource " << _name << " was unlocked.\r\n";
    }

    else
    {
        std::cerr << "Error: Resource availibility not known.";
    }
    
}