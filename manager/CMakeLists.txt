project(VideoManager)
cmake_minimum_required(VERSION 3.5)
find_package( OpenCV REQUIRED )
find_package(CURL)
find_package( Boost 1.52 COMPONENTS system thread REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


add_executable(VideoManager manager.cpp)

target_link_libraries(VideoManager ${CURL_LIBRARIES} ${Boost_LIBRARIES} ${OpenCV_LIBS} player pthread)
target_include_directories(VideoManager INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})