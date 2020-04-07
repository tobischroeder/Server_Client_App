/*
    Header File for the class Resource
*/
#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <mutex>
#ifndef RESOURCE_AVAILABILITY_H
#define RESOURCE_AVAILABILITY_H
#include "resource_availability.hpp"
#endif


class Resource
{
    public:
    Resource(std::string name);
    int getViews() const;
    void incrementViews();
    std::string getName() const;
    void setName(std::string name);
    void toggleAvailability();
    std::string getAvailability() const;

    private:
    int _views;
    std::string _name;
    Availability _availability;
    std::mutex _resource_mutex;
};

#endif