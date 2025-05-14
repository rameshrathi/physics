cmake_minimum_required(VERSION 3.30)
project(TradeSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/external/boost")

find_package(Boost 1.88 CONFIG REQUIRED
        COMPONENTS system json thread filesystem
)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_executable(trade_simulator
        src/main.cpp
        src/WebsocketClient.cpp
)

target_include_directories(trade_simulator PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIR}
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
)

