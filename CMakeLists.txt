cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(cmake_and_cuda LANGUAGES CXX CUDA)

find_package(CUDA  REQUIRED)
# include_directories("${CUDA_INCLUDE_DIRS}")

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
    src/heat_diffusion.cu
    src/cuda_gl_resource.cpp
    src/style_transform.cu
    src/device_buffer.h
    src/sim_env.cpp
)

add_executable(${TARGET} ${SRC_FILES})
message(CUDA_INCLUDE_DIRS="${CUDA_INCLUDE_DIRS}")
target_include_directories(${TARGET} PRIVATE
    ${CUDA_INCLUDE_DIRS}
    src
    imgui
    imgui/backends)

target_link_libraries(${TARGET} 
    ${CMAKE_DL_LIBS}
    glfw
    GL)

set_target_properties(diffeq
    PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
