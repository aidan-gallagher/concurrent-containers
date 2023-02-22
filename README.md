# Concurrent Containers Overview

Collection of containers that allow thread safe access.  
All containers are in the namespace `Concurrent`.

# Containers

- [Concurrent Queue](Queue/README.md)
- [Searchable Ring Buffer](SearchRingBuffer/README.md)

# Instructions
## How to Build
Download Git, CMake, a native build system and a C++ compiler
```
sudo apt install git
sudo apt install cmake 
sudo apt install make
sudo apt install clang
```
Clone the repository and navigate into it
```
git clone https://github.com/aidan-gallagher/concurrent-containers.git
cd concurrent-containers
```
Create a build folder and navigate into it
```
mkdir Build
cd Build
```
Generate a native build system 
```
cmake ..
```
Invoke the build system to compile the project
```
cmake --build .
```
## How to Test
Run the test suite
```
ctest
```
Or invoke individual test executables
```
bin/ConcurrentQueueTest
````

## How to Use in Project
If you are using CMake, then link to which ever part of library you require
```
target_link_libraries(YourProject PRIVATE Concurrent::Queue)
```
