# Define minimum CMake version
cmake_minimum_required(VERSION 3.20)

# Define project and languages
project(Sandbox CXX)

# Define executable sources
file(GLOB_RECURSE SANDBOX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE SANDBOX_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# Create executable
add_executable(Sandbox ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})

# Set Output directory
set_target_properties(Sandbox PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Sandbox/
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Sandbox/
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Sandbox/
)

# Link Ember and other libraries
target_link_libraries(Sandbox PRIVATE EmberEngine)

# Include directories
target_include_directories(Sandbox
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../Ember/Vendor/spdlog/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Ember/Vendor
        ${CMAKE_CURRENT_SOURCE_DIR}/../Ember/src
)

# Define preprocessor definitions
if(UNIX AND NOT APPLE)
    target_compile_definitions(Sandbox PRIVATE EMBER_PLATFORM_LINUX)
elseif(WIN32)
    target_compile_definitions(Sandbox PRIVATE EMBER_PLATFORM_WINDOWS)
endif()

# Set C++ standard
target_compile_features(Sandbox PRIVATE cxx_std_17)
