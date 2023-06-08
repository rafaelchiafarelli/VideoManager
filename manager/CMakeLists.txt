project(VideoManager)
cmake_minimum_required(VERSION 3.5)
find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


add_executable(VideoManager manager.cpp)

target_link_libraries(VideoManager ${OpenCV_LIBS})
target_include_directories(VideoManager INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})