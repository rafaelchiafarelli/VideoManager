project(player)
cmake_minimum_required(VERSION 3.5)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${PROJECT_SOURCE_DIR} )

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)

add_library(player ${SRC_FILES})

target_link_libraries(player ${OpenCV_LIBS})
target_include_directories(player INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

