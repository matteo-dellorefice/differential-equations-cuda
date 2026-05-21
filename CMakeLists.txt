cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(cmake_and_cuda LANGUAGES CXX CUDA)
 
include(CTest)

set(TARGET diffeq)

set(SRC_FILES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_stdlib.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_glfw.cpp
    src/main.cpp
    src/hdbuf.cu
    src/heat.cu
)

add_executable(${TARGET} ${SRC_FILES})

target_include_directories(${TARGET} PRIVATE
    src
    imgui
    imgui/backends)

target_link_libraries(${TARGET} 
    ${CMAKE_DL_LIBS}
    glfw
    GL)

set_target_properties(diffeq
    PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
