cmake_minimum_required(VERSION 3.22)
project(CS4380-Project1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

# Download and set up GoogleTest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG main
)
FetchContent_MakeAvailable(googletest)

# Include GoogleTest in the include directories
include_directories(${googletest_SOURCE_DIR}/include)

# Create executable emu4380 from main.cpp and emu4380.cpp
add_executable(emu4380 src/main.cpp src/emu4380.cpp)

# Create executable runTests from myTests.cpp
add_executable(runTests src/myTests.cpp src/emu4380.cpp)

# Link runTests with gtest_main
target_link_libraries(runTests gtest gtest_main)
