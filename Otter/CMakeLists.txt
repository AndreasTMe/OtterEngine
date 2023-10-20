project(Otter VERSION 0.1.0)

message(STATUS "Configuring ${PROJECT_NAME} library...")

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS includes/*.h)
list(REMOVE_ITEM HEADERS includes/Otter.PCH.h)

#add_subdirectory()

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
#target_link_libraries(${PROJECT_NAME} PRIVATE ...)
target_precompile_headers(${PROJECT_NAME} PRIVATE includes/Otter.PCH.h)
target_include_directories(${PROJECT_NAME} PRIVATE includes)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:OTR_DEBUG>
        $<$<CONFIG:Editor>:OTR_EDITOR>
        $<$<CONFIG:Runtime>:OTR_RUNTIME>
)