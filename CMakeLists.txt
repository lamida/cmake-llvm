cmake_minimum_required(VERSION 3.19)
project(cmake_llvm)

set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp)

# Add LLVM
# See https://llvm.org/docs/CMake.html#embedding-llvm-in-your-project:w
# See https://stackoverflow.com/questions/57780600/how-is-cmake-finding-my-llvm-cmake-configuration on how cmake can find LLVM
find_package(LLVM 13.0.0 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Find the libraries that correspond to the LLVM components that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link against LLVM libraries
target_link_libraries(main ${llvm_libs})

# Google Test
# Get Google Test
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

# Enable Testing
enable_testing()
add_executable(
        hello_test
        tests/hello_test.cc
)
target_link_libraries(
        hello_test
        gtest_main
)
include(GoogleTest)
gtest_discover_tests(hello_test)