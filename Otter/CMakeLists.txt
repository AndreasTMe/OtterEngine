project(Otter VERSION 0.1.0)

message(STATUS "Configuring ${PROJECT_NAME} library...")
set(GRAPHICS_API_FOUND 0)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS includes/*.h)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

find_package(Vulkan)
if (Vulkan_FOUND)
    message(STATUS "Vulkan found, linking ${PROJECT_NAME} with Vulkan...")

    target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
    target_compile_definitions(${PROJECT_NAME} PRIVATE OTR_GRAPHICS_VULKAN_ENABLED=1)

    set(GRAPHICS_API_FOUND 1)
else ()
    message(STATUS "Vulkan not found. If you want to use Vulkan, please install it or add it to your path variables.")
endif ()

if (NOT GRAPHICS_API_FOUND)
    get_target_property(PREPROCESSORS ${PROJECT_NAME} COMPILE_DEFINITIONS)
    foreach (PREPROCESSOR ${PREPROCESSORS})
        if (PREPROCESSOR MATCHES "^OTR_GRAPHICS_")
            set(GRAPHICS_API_FOUND 1)
            break()
        endif ()
    endforeach ()

    if (NOT GRAPHICS_API_FOUND)
        message(WARNING "No graphics API found! Application will not initialise.")
        target_compile_definitions(${PROJECT_NAME} PRIVATE OTR_GRAPHICS_NULL=1)
    endif ()
endif ()

target_include_directories(${PROJECT_NAME} PRIVATE includes)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:OTR_DEBUG>
        $<$<CONFIG:Editor>:OTR_EDITOR>
        $<$<CONFIG:Runtime>:OTR_RUNTIME>
)