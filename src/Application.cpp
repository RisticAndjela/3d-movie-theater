#include "Application.h"
#include <glad/glad.h>
#include <iostream>
#include "Camera.h"
#include "HUD.h"
#include <filesystem>
#include <iostream>

static const double TARGET_FPS = 75.0;
static const double FRAME_TIME = 1.0 / TARGET_FPS;
Camera camera;
double lastFrameTime;

Application::Application(int w, int h): width(w), height(h),camera(glm::vec3(0.0f, 1.6f, 0.0f)),lastFrameTime(0.0),hud("assets/shaders/hud_quad.vs", "assets/shaders/hud_quad.fs"){

    if (!std::filesystem::exists("assets/shaders/hud_quad.vs")) {
        std::cerr << "Vertex shader fajl ne postoji!\n";
    }

    if (!std::filesystem::exists("assets/shaders/hud_quad.fs")) {
        std::cerr << "Fragment shader fajl ne postoji!\n";
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    window = glfwCreateWindow(mode->width, mode->height, "Cinema 3D", monitor, nullptr);
    if (!window) {
        std::cout << "Ne mogu da kreiram GLFW prozor!\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Ne mogu da inicijalizujem GLAD!\n";
        glfwTerminate();
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    hud.init();
}

void Application::run() {

    lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {

        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        processInput(deltaTime);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "GL Error: " << err << std::hex << err << std::endl;
        }
        hud.draw(width, height);

        glfwSwapBuffers(window);
        glfwPollEvents();

        double elapsed = glfwGetTime() - currentTime;
        if (elapsed < FRAME_TIME) {
            glfwWaitEventsTimeout(FRAME_TIME - elapsed);
        }
    }
    glfwTerminate();
}

void Application::processInput(double deltaTime) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    float forward = 0.0f;
    float right = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    forward += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  forward -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) right += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  right -= 1.0f;

    camera.processKeyboard(forward, right, (float)deltaTime);
    camera.clampToBounds();
}
