cmake_minimum_required(VERSION 3.10)
project(TradeSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# Boost
set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/external/boost")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib")

find_package(Boost REQUIRED
        COMPONENTS system thread filesystem chrono date_time atomic regex
)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# FTX-UI
add_subdirectory(external/ftxui)

add_executable(trade_simulator
        src/main.cpp
        src/WebsocketClient.cpp
)

target_include_directories(trade_simulator PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(trade_simulator PRIVATE
        ${Boost_LIBRARIES}
        OpenSSL::SSL OpenSSL::Crypto
        Threads::Threads
        ftxui::screen
        ftxui::dom
        ftxui::component
)