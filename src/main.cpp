
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "IconsFontAwesome6.h"
#include "sim_env_heat.h"
#include "sim_env_wave.h"
#include "buffer_printer.h"

#include <math.h>
#include <stdio.h>
#include <GLFW/glfw3.h> 
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include <memory>
#include <cstdlib>
#include <iostream>
#include <iomanip>
// #include <helper_cuda.h>
// #include <helper_cuda_gl.h>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

inline void findCudaGLDevice()
{
    std::cout << "[RENDERER] " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "[VERSION] " << glGetString(GL_VERSION) << std::endl; 

    unsigned int pCudaDeviceCount = 0;
    int pCudaDevices[4];
    cudaGLGetDevices(&pCudaDeviceCount, pCudaDevices, 4, cudaGLDeviceListAll);

    if (pCudaDeviceCount > 0) {
        cudaSetDevice(pCudaDevices[0]);
    } else {
        printf("No CUDA devices associated with the active GL context.\n");
    }

    int device;
    cudaDeviceProp props;
    cudaGetDevice(&device);
    cudaGetDeviceProperties(&props, device);

    std::cout << std::setw(50) << std::left << "::: maxBlocksPerMultiProcessor" << props.maxBlocksPerMultiProcessor << std::endl;
    std::cout << std::setw(50) << std::left << "::: maxThreadsPerMultiProcessor" << props.maxThreadsPerMultiProcessor << std::endl;
    std::cout << std::setw(50) << std::left << "::: maxThreadsPerBlock" << props.maxThreadsPerBlock << std::endl;
    std::cout << std::setw(50) << std::left << "::: sharedMemPerMultiprocessor" << props.sharedMemPerMultiprocessor << std::endl;
    std::cout << std::setw(50) << std::left << "::: sharedMemPerBlock" << props.sharedMemPerBlock << std::endl;
}

#define WIDTH 800
#define HEIGHT 600

int main(int, char**)
{
    putenv((char *) "__NV_PRIME_RENDER_OFFLOAD=1");
    putenv((char *) "__GLX_VENDOR_LIBRARY_NAME=nvidia");

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // // Create window with graphics context
    float main_scale = 1.1 * ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    // printf("main_scale: %f\n", main_scale);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "PDE Simulator", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    findCudaGLDevice();
    
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
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

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
    // style.FontSizeBase = 20.0f;
    // io.Fonts->AddFontDefaultVector();
    //io.Fonts->AddFontDefaultBitmap();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    io.Fonts->AddFontDefaultVector();
    // float baseFontSize = 13.0f; // 13.0f is the size of the default font. Change to the font size you use.
    // float iconFontSize = baseFontSize * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly

    // merge in icons from Font Awesome
    // static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    // ImFontConfig icons_config; 
    // icons_config.MergeMode = true; 
    // icons_config.PixelSnapH = true; 
    // // icons_config.GlyphMinAdvanceX = 8.66;
    // io.Fonts->AddFontFromFileTTF( "fonts/" FONT_ICON_FILE_NAME_FAS, .0, &icons_config);//, icons_ranges);
    // use FONT_ICON_FILE_NAME_FAR if you want regular instead of solid

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show_demo_window = true;

    // float * init = (float *) calloc(WIDTH * HEIGHT, sizeof(float));
    // int col_center = 0.6 * WIDTH;
    // int row_center = 0.5 * HEIGHT;
    // float radius = 50;

    // for (int r = 0; r < HEIGHT; r++) {
    //     for (int c = 0; c < WIDTH; c++) {
    //         float dist = sqrt((r - row_center) * (r - row_center) + (c - col_center) * (c - col_center));
    //         if (dist < radius) {
    //             init[c + r * WIDTH] = 1.;
    //         }
    //     }
    // }

    // for (int r = 1; r < HEIGHT - 1; r++) {
    //     for (int c = 1; c < 40; c++) {
    //         init[c + r * WIDTH] = 1.;
    //     }
    // }

    // for (int r = 1; r < HEIGHT - 1; r++) {
    //     for (int c = 300; c < 350; c++) {
    //         init[c + r * WIDTH + (r / 4)] = 1.;
    //     }
    // }

    // heat_diffusion_params(unsigned int width, unsigned int height, 
    //     float dx, float alpha)
    // heat_diffusion_params *params = new heat_diffusion_params(WIDTH, HEIGHT, 0.1, 0.5);
    // sim_env env(params);
    // cudaMemcpy(env.in1.buffer, init, env.params->width * env.params->height * sizeof(float), cudaMemcpyHostToDevice);

    // sim_env_heat env(WIDTH, HEIGHT, 0.1, 1., 0.5);
    sim_env_wave env(WIDTH, HEIGHT, 1, 1., 1.);
    // cudaMemcpy(&env.in[0], init, env.width * env.height * sizeof(float), cudaMemcpyHostToDevice);
    // cudaMemcpy(&env.in[1], init, env.width * env.height * sizeof(float), cudaMemcpyHostToDevice);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwWaitEventsTimeout(1. / 60);
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
        env.render();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame(); 

        // ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Heat diffusion simulation", nullptr, ImGuiWindowFlags_MenuBar);
        
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {
                if (ImGui::MenuItem("New")) {}
                if (ImGui::MenuItem("Open", "Ctrl+O")) {}
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        static bool sim_playing = false;

        if (ImGui::Button(sim_playing ? "Pause" : "Play")) {
            sim_playing = !sim_playing;
        }

        ImGui::SameLine();
        ImGui::Text("(FPS %.1f) Sim step: %d, %.1f x %.1f [m], t = %f [s], dt = %f [s]", 
            ImGui::GetIO().Framerate, 
            env.step,
            env.width * env.dx, 
            env.height * env.dx,
            env.dt * env.step,
            env.dt);
        ImGui::Text("dt upper bound = %f [s], c = %.2f [m/s]",
            env.max_dt(),
            env.c);
        // ImGui::Text("size = %d x %d", env.res.width, env.res.height);
        ImGui::Image(env.res.texture_id, ImVec2(WIDTH, HEIGHT));
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

        if (sim_playing) {
            if (env.step % 20 == 0)
                print_random_gaussian(env.in[0], env.in[1], WIDTH, HEIGHT);
            env.run(1);
        }
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
