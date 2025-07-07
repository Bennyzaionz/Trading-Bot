cmake_minimum_required(VERSION 3.14)
project(AlgorithmicTrading)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set the parent target for easy change if desired later (not locked in to project name)
set(PARENT_TARGET ${PROJECT_NAME})
set(LIBRARY_NAME algo_trading)

# goes to src/CMakeLists.txt
add_subdirectory(src)

# create executable file
add_executable(${PARENT_TARGET} test/test.cpp)

message(STATUS "EXECUTABLE NAME: ${PARENT_TARGET}.exe")  # debug output

# link library created in src/CMakeLists.txt
target_link_libraries(${PARENT_TARGET} PRIVATE ${LIBRARY_NAME})

# include the header files in parent target
target_include_directories(${PARENT_TARGET} PRIVATE ${PROJECT_SOURCE_DIR}/include)