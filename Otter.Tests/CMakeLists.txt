project(Otter.Tests VERSION 0.1.0)

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        OTR_INTERNALS_TEST_VISIBLE=1

        PRIVATE
        $<$<CONFIG:Debug>:OTR_DEBUG>
        $<$<CONFIG:Editor>:OTR_EDITOR>
        $<$<CONFIG:Runtime>:OTR_RUNTIME>
)