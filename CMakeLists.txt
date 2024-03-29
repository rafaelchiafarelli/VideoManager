cmake_minimum_required(VERSION 3.5)
project( FullTest )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package( OpenCV REQUIRED )

find_package( Boost 1.52 COMPONENTS system thread REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/player)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/manager)

add_executable( FullTest tests.cpp )

add_subdirectory(player)
add_subdirectory(manager)


find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

file(COPY data.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ )
file(COPY test_player.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ )
target_link_libraries( FullTest ${Boost_LIBRARIES} ${OpenCV_LIBS} player Threads::Threads -lcurl)