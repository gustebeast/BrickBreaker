cmake_minimum_required(VERSION 3.2)
project(BrickBreaker)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,@executable_path/Frameworks/")

set(EXECUTABLE_NAME "BrickBreaker")
set(SOURCE_FILES Constants.h GraphicsRunner.cpp GraphicsRunner.h main.cpp Object.h Ball.cpp Ball.h Barrier.cpp Barrier.h Paddle.cpp Paddle.h Brick.cpp Brick.h StageBuilder.cpp StageBuilder.h)
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
find_package(SFML REQUIRED graphics window system)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
    target_link_libraries(${EXECUTABLE_NAME} ${SFML_DEPENDENCIES})
endif()


#OLD

## Change this if you need to target a specific CMake version
#cmake_minimum_required(VERSION 3.2)
#project(brickbreaker)
#
#set(CMAKE_BUILD_TYPE Release) #If you have problems check if you have SFML debug libraries or release libraries
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#
#set(BUILD_SHARED_LIBS TRUE)
#
## Detect and add SFML
#set(SFML_ROOT "/Users/gus/ClionProjects/SFML-2.3.1-osx-clang-universal/")
#set(CMAKE_MODULE_PATH "/Users/gus/ClionProjects/SFML-2.3.1-osx-clang-universal/cmake/Modules" ${CMAKE_MODULE_PATH})
#find_package(SFML 2 COMPONENTS window graphics system)
#include_directories(${SFML_INCLUDE_DIR})
#
## Define sources and executable
#include_directories(${PROJECT_SOURCE_DIR})
#set(EXECUTABLE_NAME "BrickBreaker")
#add_executable(${EXECUTABLE_NAME} Constants.h GraphicsRunner.cpp GraphicsRunner.h main.cpp Object.h Ball.cpp Ball.h Barrier.cpp Barrier.h Paddle.cpp Paddle.h Brick.cpp Brick.h StageBuilder.cpp StageBuilder.h)
#
#target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})