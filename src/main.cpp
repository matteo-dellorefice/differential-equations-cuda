
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "cuda_gl_resource.h"
#include "hdbuf.cuh"
#include "heat.cuh"
#include "style_transform.cuh"

#include <math.h>
#include <stdio.h>
#include <GLFW/glfw3.h> 
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

// #include <helper_cuda.h>
// #include <helper_cuda_gl.h>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

inline int findCudaGLDevice()
{
    unsigned int pCudaDeviceCount;
    int pCudaDevices[4];
    
    cudaGLGetDevices(&pCudaDeviceCount, pCudaDevices, 4, cudaGLDeviceListAll);

    printf("Found %d CUDA devices associated with the current OpenGL context.\n", pCudaDeviceCount);

    if (pCudaDeviceCount > 0) {
        // Output the specific device IDs found
        for(int i = 0; i < pCudaDeviceCount; ++i) {
            printf("Associated Device ID: %d\n", pCudaDevices[i]);
        }

        // Set the primary/first device for CUDA operations
        cudaSetDevice(pCudaDevices[0]);
        printf("Set active CUDA device to: %d\n", pCudaDevices[0]);
    } else {
        printf("No CUDA devices associated with the active GL context.\n");
    }
}

#define WIDTH 800
#define HEIGHT 600

int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // // Create window with graphics context
    // float main_scale = 1.5 * ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    // printf("main_scale: %f\n", main_scale);
    GLFWwindow* window = glfwCreateWindow(1280, 800, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    // ImGuiStyle& style = ImGui::GetStyle();
    // style.ScaleAllSizes(main_scale);
    // style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If fonts are not explicitly loaded, Dear ImGui will select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
    //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small threshold.
    // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefaultVector();
    //io.Fonts->AddFontDefaultBitmap();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show_demo_window = true;

    // findCudaGLDevice();

    
    hdbuf_t in = hdbuf_create(WIDTH * HEIGHT);
    hdbuf_t out = hdbuf_create(WIDTH * HEIGHT);
    int col_center = WIDTH / 2;
    int row_center = HEIGHT / 2;
    float radius = HEIGHT / 2;
    for (int r = 0; r < HEIGHT; r++) {
        for (int c = 0; c < WIDTH; c++) {
            float dist = sqrt((r - row_center) * (r - row_center) + (c - col_center) * (c - col_center));
            if (dist < radius) {
                in.host[c + r * WIDTH] = 1.;
            }
        }
    }
    hdbuf_memcpy(in, cudaMemcpyHostToDevice);
    // for (int i = 0; i < 100; i++) {
    //     heat(in, out, WIDTH, HEIGHT);
    //     hdbuf_swap(&in, &out);
    // }
    // hdbuf_memcpy(out, cudaMemcpyDeviceToHost);
    cuda_gl_resource resource(WIDTH, HEIGHT);

    unsigned int *greybuf;
    cudaMalloc((void**) &greybuf, WIDTH * HEIGHT * sizeof(unsigned int));
    transform_1channel(in.device, greybuf, WIDTH, HEIGHT);
    resource.transfer(greybuf);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwWaitEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Begin("Texture test2");
        // ImGui::Text("pointer = %x", resource.texture_id);
        // ImGui::Text("size = %d x %d", resource.width, resource.height);
        ImGui::Image(resource.texture_id, ImVec2(WIDTH, HEIGHT));
        ImGui::End();

        // Rendering
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
