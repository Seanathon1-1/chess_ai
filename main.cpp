// chess_ai.cpp : Defines the entry point for the application.
//

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "gl/glew.h"
#include "GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "game.h"

using namespace std;

int main() {
    // Initialize GLFW.
    if (!glfwInit())
    {
        cerr << "Failed to initialize GLFW" << endl;
        return NULL; 
    }

    // Create the GLFW window.
    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL);
    if (!window) 
    {
        cerr << "Failed to open GLFW window." << endl;
        glfwTerminate(); 
        exit(EXIT_FAILURE);
    }

    // Make the context of the window.
    glfwMakeContextCurrent(window);

    // Initialize GLEW.
    if (glewInit())
    {
        cerr << "Failed to initialize GLEW" << endl;
        return NULL;
    }

    // Set swap interval to 1.
    glfwSwapInterval(0);
        
    // imgui init
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    ImGui::StyleColorsDark();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    float vertices[] = {
            -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
             0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f };

    unsigned int frame_buffer_object;
    glGenFramebuffers(1, &frame_buffer_object);
    //glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_object);

    glBindVertexArray(0);

    Game game(frame_buffer_object);
    while (!glfwWindowShouldClose(window)) {
        //cout << ImGui::GetWindowSize().x << ", " << ImGui::GetWindowSize().y << endl;
        
        glfwPollEvents();

        // starts imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // imgui components
        ImGui::Begin("Hello world!");
        ImGui::Text("Hey window!");
        ImGui::End();

        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        
        game.render();



        // Render imgui into screen
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
     
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //board.play();

    glfwTerminate();
    return 0;
}