# Define minimum CMake version
cmake_minimum_required(VERSION 3.20)

# Define project and languages
project(Ember CXX)

# Define options
option(EMBER_ENABLE_ASSERTS "Enable Debugging Assertions" ON)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Set Options
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")

# Define library sources
file(GLOB_RECURSE EMBER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE EMBER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# Create library
if(BUILD_SHARED_LIBS)
    add_library(Ember SHARED ${EMBER_SOURCES} ${EMBER_HEADERS})
else()
    add_library(Ember STATIC ${EMBER_SOURCES} ${EMBER_HEADERS})
endif()

# Add subdirectory for dependencies
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/spdlog)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/GLFW)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/Glad)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGui)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glm)

# Include directories
target_include_directories(Ember
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/GLFW/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/Glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGui
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glm/glm
)

# Set output directories
set_target_properties(Ember 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Ember/
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Ember/
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Ember/
)

set_target_properties(spdlog 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/spdlog/
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/spdlog/
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/spdlog/
)

set_target_properties(glfw 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/GLFW/
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/GLFW/
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/GLFW/
)

set_target_properties(Glad
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Glad/
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Glad/
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Glad/
)

set_target_properties(ImGui 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/ImGui/
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/ImGui/
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ImGui/
)

set_target_properties(glm 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/glm/
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/glm/
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/glm/
)

# Find Packages
find_package(OpenGL REQUIRED)

# Define preprocessor definitions
target_compile_definitions(Ember PRIVATE GLFW_INCLUDE_NONE)

if(UNIX AND NOT APPLE)
    target_compile_definitions(Ember PRIVATE EMBER_PLATFORM_LINUX EMBER_BUILD_LIB)
elseif(WIN32)
    target_compile_definitions(Ember PRIVATE EMBER_PLATFORM_WINDOWS EMBER_BUILD_LIB)
endif()

if(EMBER_ENABLE_ASSERTS)
    target_compile_definitions(Ember PRIVATE EMBER_ENABLE_ASSERTS)
endif()

# Link Libraries
target_link_libraries(Ember PRIVATE spdlog glfw dl pthread OpenGL::GL Glad ImGui glm)

# Precompiled headers
target_precompile_headers(Ember PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/Emberpch.h)

# Set C++ standard
target_compile_features(Ember PUBLIC cxx_std_17)
