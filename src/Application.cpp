#include "Application.h"
#include <glad/glad.h>
#include <iostream>
#include "Camera.h"
#include <filesystem>
#include "../assets/models/OverlayRectangle.h"

#ifdef _WIN32
#include <windows.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
static std::filesystem::path getExecutableDir() {
    char buf[MAX_PATH];
    GetModuleFileNameA(NULL, buf, MAX_PATH);
    return std::filesystem::path(buf).parent_path();
}
#else
static std::filesystem::path getExecutableDir() {
    return std::filesystem::current_path();
}
#endif

static const double TARGET_FPS = 75.0;
static const double FRAME_TIME = 1.0 / TARGET_FPS;
Camera camera;
double lastFrameTime;

Application::Application(int w, int h)
    : width(w), height(h), camera(glm::vec3(0.0f, 1.6f, 0.0f)), lastFrameTime(0.0) {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    window = glfwCreateWindow(mode->width, mode->height, "Cinema 3D", monitor, nullptr);
    //window = glfwCreateWindow(800, 600, "Cinema 3D", nullptr, nullptr);

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

    InitOverlayRectangle();
    auto exeDir = getExecutableDir();
    exeDir = exeDir.parent_path();
    exeDir = exeDir.parent_path();
    exeDir = exeDir.parent_path();
    std::string vertPath = (exeDir / "assets\\shaders\\overlay.vert").string();
    std::string fragPath = (exeDir / "assets\\shaders\\overlay.frag").string();
    overlayShader = CreateShaderProgram(vertPath.c_str(), fragPath.c_str());
    overlayVAO = GetOverlayRectangleVAO();
    overlayInitialized = true;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    std::string seatVert = (exeDir / "assets\\shaders\\seat.vert").string();
    std::string seatFrag = (exeDir / "assets\\shaders\\seat.frag").string();
    seatShader = CreateShaderProgram(seatVert.c_str(), seatFrag.c_str());
    Seat3D::initCube();
    initSeats();
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

        if (overlayInitialized) {
            glDisable(GL_DEPTH_TEST); // overlay uvek na vrhu

            // Dodaj: isključi culling za 2D overlay
            glDisable(GL_CULL_FACE);

            glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glm::mat4 proj = glm::ortho(0.0f, float(width), 0.0f, float(height));

            glUseProgram(overlayShader);
            glUniformMatrix4fv(glGetUniformLocation(overlayShader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

            DrawOverlayRectangle(overlayShader, overlayVAO);
           
            glDisable(GL_BLEND);

            // Vrati stanje cull-a ako ti treba za 3D scenu
            glEnable(GL_CULL_FACE);

            glEnable(GL_DEPTH_TEST);
        }

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);

        for (const auto& seat : seats3D) {
            seat.draw(seatShader, view, projection);
        }

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

    // --- Rotacija AWSD ---
    camera.processRotationKeyboard(window, (float)deltaTime);

    // --- Pomeranje strelicama ---
    float forward = 0.0f;
    float right = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    forward += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  forward -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) right += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  right -= 1.0f;

    camera.processKeyboard(forward, right, (float)deltaTime);

    // --- Clamp po granicama i sedistima ---
    camera.clampToBounds();
    camera.clampToSeats(seats3D);
}

void Application::initSeats() {
    seats3D.clear();
    int rows = 5;
    int cols = 10;
    float spacing = 1.0f;  // razmak između sedišta
    float startX = -cols / 2.0f * spacing;
    float startZ = -5.0f; // udaljenost od platna

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            glm::vec3 pos(
                startX + c * spacing,
                0.25f, // visina sedišta (centar kvadra)
                startZ - r * spacing
            );
            seats3D.emplace_back(pos);
        }
    }
}
