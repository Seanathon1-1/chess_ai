// chess_ai.cpp : Defines the entry point for the application.
//

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "gl/glew.h"
#include "GLFW/glfw3.h"
#include "game.h"

#define WIN_TITLE "Chess AI"
#define WIN_WIDTH  800
#define WIN_HEIGHT 600

using namespace std;

int main() {
    // Initialize GLFW.
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return NULL;
    }

    // Create the GLFW window.
    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL);
    if (!window) 
    {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Make the context of the window.
    glfwMakeContextCurrent(window);

    // Initialize GLEW.
    if (glewInit())
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return NULL;
    }

    // Set swap interval to 1.
    glfwSwapInterval(0);
        
    // imgui init
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    ImGui::StyleColorsDark();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    Game game;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // starts imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // imgui components
        ImGui::Begin("Hello world!");
        ImGui::Text("Hey window!");
        ImGui::End();

        game.render();
        ImGui::Begin("Play window", 0, ImGuiWindowFlags_NoTitleBar);
        char move[16] = "";
        bool moveEntered = ImGui::InputText("Make Move", move, 16, ImGuiInputTextFlags_EnterReturnsTrue);
        if (moveEntered) game.makeUserMove(move);
        ImGui::End();

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


    return 0;
}