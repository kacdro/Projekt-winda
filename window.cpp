#include "window.hpp"
#include "elevator.hpp"

#include <functional>
#include <stdio.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include "glfw3.h" // Will drag system OpenGL headers

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif



const auto WIDTH = 800;
const auto HEIGHT = 600;
const auto TITLE = "Symulator windy";

static GLFWwindow* windowHandler = nullptr;
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


namespace window
{

    void create()
    {
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            return;

#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
        const char* glsl_version = "#version 100";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            
#else
    // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130"; //wersja jêzyka GLSL
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

        windowHandler = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);

        if (windowHandler == NULL)
            return;
        glfwMakeContextCurrent(windowHandler);
        glfwSwapInterval(1); // Enable vsync

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;//odpowiada za eventy z klawiatury

        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(windowHandler, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }


    /**
     * @brief run
     * external\include\imgui\imgui_demo.cpp
     */
    void run()
    {
        Elevator elevator;

        while (!glfwWindowShouldClose(windowHandler))
        {
            
            glfwPollEvents(); //przyjmuje i przechowuje eventy

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            elevator.draw();

            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(windowHandler, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.25f, 0.27f, 0.29f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(windowHandler);
        }
    }

  
    void close()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(windowHandler);
        glfwTerminate();
    }

}