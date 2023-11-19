project(Otter VERSION 0.1.0)

message(STATUS "Configuring ${PROJECT_NAME} library...")

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS includes/*.h)
list(REMOVE_ITEM HEADERS includes/Otter.PCH.h)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

find_package(Vulkan)
if (Vulkan_FOUND)
    message(STATUS "Vulkan found, linking ${PROJECT_NAME} with Vulkan...")

    target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
    target_compile_definitions(${PROJECT_NAME} PRIVATE OTR_VULKAN_ENABLED=1)
else ()
    message(STATUS "Vulkan not found. If you want to use Vulkan, please install it or add it to your path variables.")

    target_compile_definitions(${PROJECT_NAME} PRIVATE OTR_VULKAN_ENABLED=0)
endif ()

target_precompile_headers(${PROJECT_NAME} PRIVATE includes/Otter.PCH.h)
target_include_directories(${PROJECT_NAME} PRIVATE includes)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:OTR_DEBUG>
        $<$<CONFIG:Editor>:OTR_EDITOR>
        $<$<CONFIG:Runtime>:OTR_RUNTIME>
)