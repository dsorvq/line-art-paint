add_executable(gui_example 
    gui_example.cpp
    ../src/matrix_utils.cpp
    ../src/graph_utils.cpp
    ../src/stb.cpp
)   # probably not the best way to do it

target_link_libraries(gui_example PRIVATE imgui glfw)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
target_link_libraries(gui_example PRIVATE OpenGL::GL)

# target_include_directories(gui_example PRIVATE path/to/include)

