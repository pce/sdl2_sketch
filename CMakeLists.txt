cmake_minimum_required(VERSION 3.7)
project(SDL2Test)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Common settings
set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Source files
set(SOURCE_FILES src/main.cpp)

# Determine if we are building for Web (Emscripten)
if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  set(USE_FLAGS "-s USE_SDL=2")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
else()
  # Native build
  find_package(SDL2 REQUIRED)
  include_directories(${SDL2_INCLUDE_DIRS})
endif()

# Add executable
add_executable(sketch ${SOURCE_FILES})

# Link libraries
if(NOT EMSCRIPTEN)
  string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
  target_link_libraries(sketch ${SDL2_LIBRARIES})
endif()
