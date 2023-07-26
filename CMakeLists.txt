cmake_minimum_required (VERSION 3.0)
project (line-art-paint)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")

add_compile_options()

include_directories(ext/stb include)

add_executable(
    ${CMAKE_PROJECT_NAME}
    src/main.cpp 
    src/stb.cpp
    src/matrix_utils.cpp
    src/graph_utils.cpp
    src/painter.cpp
)

# tests
option(BUILD_TESTS "Build examples" YES)
if(BUILD_TESTS)
    add_subdirectory(googletest)

    add_executable(
        ${CMAKE_PROJECT_NAME}_test
        test/matrix_test.cpp
        src/stb.cpp
    )

    target_link_libraries(${CMAKE_PROJECT_NAME}_test gtest gtest_main)
    enable_testing()
    add_test(NAME LineArtPaintTests COMMAND ${CMAKE_PROJECT_NAME}_test)
endif()

# examples
option(BUILD_EXAMPLES "Build examples" OFF)
if(BUILD_EXAMPLES)
    add_subdirectory(ext/imgui)
    add_subdirectory(ext/glfw)
    include_directories(ext/glfw/include)

    add_subdirectory(examples)
endif()

add_custom_target(run
    COMMAND ${CMAKE_PROJECT_NAME}
    DEPENDS ${CMAKE_PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
