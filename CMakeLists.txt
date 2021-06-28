cmake_minimum_required(VERSION 3.19)
project(cmake_llvm)

set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp)

# Add LLVM
# See https://llvm.org/docs/CMake.html#embedding-llvm-in-your-project:w
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Find the libraries that correspond to the LLVM components that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link against LLVM libraries
target_link_libraries(main ${llvm_libs})
