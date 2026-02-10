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

        float spacingZ = 1.0f;
        float stepHeight = 0.3f;
        float startX = -((10 - 1) * 1.0f) / 2.0f;
        float endX = ((10 - 1) * 1.0f) / 2.0f;

        for (int r = 1; r <= 5; ++r) {
            drawSteps(startX, endX, r, stepHeight, spacingZ, view, projection);
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
    float spacingX = 1.0f;   // razmak između sedišta u redu
    float spacingZ = 1.0f;   // razmak između redova u dubinu
    float stepHeight = 0.3f; // koliko svaki red raste u visinu

    float startX = -((cols - 1) * spacingX) / 2.0f; // centriranje oko Z ose

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            glm::vec3 pos(
                startX + c * spacingX, // X: levo-desno
                r * stepHeight,        // Y: visina (stepeničasto)
                -r * spacingZ - 5.0f   // Z: udaljenost od platna
            );
            seats3D.emplace_back(pos);
        }
    }
}

void Application::drawSteps(float xStart, float xEnd, int rowIndex, float stepHeight, float spacingZ,
    const glm::mat4& view, const glm::mat4& projection)
{
    float yPrev = rowIndex * stepHeight;        // dno sedišta prethodnog reda
    float zPrev = -rowIndex * spacingZ - 4.75f; // Z prethodnog reda

    glm::vec3 topColor(0.2f, 0.1f, 0.15f);
    glm::vec3 frontColor = topColor * 0.5f;

    float stepDepth = spacingZ ;
    float seatHeight = 0.3f; 
    float horizontalHeight = 0.01f;    // debljina horizontalne ploče

    // --- horizontalna ravan ---
    glm::mat4 model = glm::mat4(1.0f);
    float yStep = yPrev - seatHeight - 0.3f; // POD počinje na dnu kocke
    model = glm::translate(model, glm::vec3((xStart + xEnd) / 2.0f, yStep + horizontalHeight / 2.0f, zPrev + stepDepth / 2.0f));
    model = glm::scale(model, glm::vec3(xEnd - xStart + 4.0f, horizontalHeight, stepDepth));
    glUniformMatrix4fv(glGetUniformLocation(seatShader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3fv(glGetUniformLocation(seatShader, "color"), 1, &topColor[0]);
    glBindVertexArray(Seat3D::cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // --- vertikalna ploča ---
    model = glm::mat4(1.0f);
    float heightDiff = stepHeight;
    model = glm::translate(model,glm::vec3((xStart + xEnd) / 2.0f,yStep - heightDiff / 2.0f, zPrev + stepDepth));
    model = glm::scale(model,glm::vec3(xEnd - xStart + 4.0f, heightDiff, 0.01f));
    glUniformMatrix4fv(glGetUniformLocation(seatShader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3fv(glGetUniformLocation(seatShader, "color"), 1, &frontColor[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

}

