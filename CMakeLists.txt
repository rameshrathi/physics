cmake_minimum_required(VERSION 3.30)
project(TradeSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/external/boost")
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")

find_package(Boost 1.88 CONFIG REQUIRED
        COMPONENTS system json thread filesystem
)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_library(OpenGL_FRAMEWORK OpenGL)

add_executable(trade_simulator
        src/main.cpp
        src/WSClient.cpp
)

# ImGUI
add_subdirectory(external/imgui)
add_subdirectory(external/glfw)

target_include_directories(trade_simulator PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
)

target_link_libraries(trade_simulator PRIVATE
    Boost::system
    Boost::json
    Boost::thread
    Boost::filesystem
    ${Boost_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
    imgui
    glfw
    ${OpenGL_FRAMEWORK}
)