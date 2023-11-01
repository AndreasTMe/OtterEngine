project(Otter VERSION 0.1.0)

message(STATUS "Configuring ${PROJECT_NAME} library...")

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS includes/*.h)
list(REMOVE_ITEM HEADERS includes/Otter.PCH.h)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

find_package(Vulkan REQUIRED)
if (Vulkan_FOUND)
    message(STATUS "Vulkan found, linking ${PROJECT_NAME} with Vulkan...")

    target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
endif ()

target_precompile_headers(${PROJECT_NAME} PRIVATE includes/Otter.PCH.h)
target_include_directories(${PROJECT_NAME} PRIVATE includes)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:OTR_DEBUG>
        $<$<CONFIG:Editor>:OTR_EDITOR>
        $<$<CONFIG:Runtime>:OTR_RUNTIME>
)