cmake_minimum_required(VERSION 3.26 FATAL_ERROR)

project(OtterEngine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SUPPRESS_REGENERATION ON)
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_CONFIGURATION_TYPES "Debug;Editor;Runtime" CACHE STRING "" FORCE)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, defaulting to \"Editor\"")
    set(CMAKE_BUILD_TYPE "Editor" CACHE STRING "" FORCE)
endif ()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

message(STATUS "Setting output directories...")
foreach (CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER}
            "${CMAKE_SOURCE_DIR}/bin/${CONFIG_TYPE}-${CMAKE_SYSTEM_NAME}/${PROJECT_NAME}/$<0:>")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER}
            "${CMAKE_SOURCE_DIR}/bin-lib/${CONFIG_TYPE}-${CMAKE_SYSTEM_NAME}/${PROJECT_NAME}/$<0:>")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER}
            "${CMAKE_SOURCE_DIR}/bin-lib/${CONFIG_TYPE}-${CMAKE_SYSTEM_NAME}/${PROJECT_NAME}/$<0:>")
endforeach ()

# Main Engine
add_subdirectory(Otter)
# Test Setup
add_subdirectory(ThirdParty/stb)
target_include_directories(Otter PRIVATE ThirdParty/stb)

# Sandbox configuration
add_subdirectory(Sandbox)
target_include_directories(Sandbox PRIVATE Otter/includes)
target_link_libraries(Sandbox PRIVATE Otter)

# Test Setup
add_subdirectory(ThirdParty/GoogleTest)
# Main Engine Tests
add_subdirectory(Otter.Tests)
target_include_directories(Otter.Tests PRIVATE ${gtest_SOURCE_DIR} ${gtest_SOURCE_DIR}/include Otter/includes)
target_link_libraries(Otter.Tests PRIVATE gtest gtest_main Otter)

if (MSVC)
    set_property(DIRECTORY Sandbox PROPERTY VS_STARTUP_PROJECT Sandbox)
endif ()