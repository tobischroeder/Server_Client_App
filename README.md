# Concurrent Server-Client Application for Resource Management

This project was created to practice TCP Server-Client communication in combination with C++ core concepts like object orientation, concurrency, memory management, smart pointers and information hiding.

## Dependencies
* due to the TCP implementation unix only
* cmake >= 3.7
* make >= 4.1
* gcc/g++ >= 7.5.0
* developed on Ubuntu 18.04.3 LTS

## Basic Build Instructions

1. Clone this Repo
2. Make a build directory in top level directory
3. Compile: `cmake .. && make`
4. Run the server: `./Server`
5. Open new console window for each client and it with: `./Client`
6. In the clients console type the resource name as it is shown in the overview(e.g. `document1`) and hit enter to allocate a resource
7. In the clients console type `unlock` to deallocate from client
8. Look at client's console to get information about connection/disconnection and allocation/deallocation


