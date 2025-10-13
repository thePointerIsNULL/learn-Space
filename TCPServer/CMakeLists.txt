cmake_minimum_required(VERSION 3.28)

project(TcpClient CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)



add_executable(TcpClient
                                main.cpp
                                TcpServer.h
                                TcpServer.cpp
                                CMByteArray.h
                                CMByteArray.cpp
                                TcpServer.h
                                TcpServer.cpp)

target_compile_definitions(TcpClient PRIVATE Linux)
