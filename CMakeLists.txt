cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(diffeq LANGUAGES CXX CUDA)

# include_directories("${CUDA_INCLUDE_DIRS}")
find_package(CUDAToolkit REQUIRED)

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
    src/cuda_gl_resource.cpp
    src/style_transform.cu
    src/buffer.h
    src/finite_differences.cu
    src/sim_env.cpp
    src/sim_env_heat.cpp
    src/sim_env_heat_solver.cu
    src/sim_env_wave.cpp
    src/sim_env_wave_solver.cu
    src/buffer_printer.cu
)

add_executable(${TARGET} ${SRC_FILES})
set_property(TARGET ${TARGET} PROPERTY CUDA_ARCHITECTURES native)

target_include_directories(${TARGET} PRIVATE
    ${CUDAToolkit_INCLUDE_DIRS}
    src
    imgui
    imgui/backends)

target_link_libraries(${TARGET} 
    ${CMAKE_DL_LIBS}
    ${CUDA_CUDART}
    glfw
    GL)

set_target_properties(diffeq
    PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
