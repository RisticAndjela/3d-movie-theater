#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include "Application.h"
#include <glad/glad.h>
#include <iostream>
#include <filesystem>
#include "../assets/models/OverlayRectangle.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef _WIN32
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
    std::string seatTexPath = (exeDir / "assets\\textures\\seat.png").string();
    Seat3D::seatTexture = LoadTexture(seatTexPath.c_str());
    if (Seat3D::seatTexture == 0) {
        std::cerr << "Warning: seat texture not loaded\n";
    }
    initSeats();
    if (!seats3D.empty()) {
        personManager = new PersonManager(seatService, seats3D[0]);
    }

    // Učitaj frejmove za platno
    std::string framesPath = (exeDir / "assets\\textures\\screen_frames").string();
    if (!screen.loadFramesFromFolder(framesPath)) {
        std::cerr << "Warning: screen frames not loaded\n";
    }
    if (screen.hasFrames()) {
        std::cout << "Screen: loaded frames for projection.\n";
    }
    else {
        std::cout << "Screen: no frames loaded (check path: " << framesPath << ")\n";
    }
    screen.setFrameDuration(1.0f / 2.0f);

    float margin = 10.0f;
    float seatWidth = 1.0f; 
    float seatDepth = 1.0f;
    int rows = seatService.getNumRows();
    int cols = seatService.getNumCols();

    float spacingX = 1.0f;
    float spacingZ = 1.0f;
    float stepHeight = 0.3f;

    float startX = -((cols - 1) * spacingX) / 2.0f - 1.0f;
    glm::vec3 minB = glm::vec3(startX - seatWidth * 0.5f - margin, 0.0f, -rows * spacingZ - 5.0f - margin);
    glm::vec3 maxB = glm::vec3(startX + (cols - 1) * spacingX + seatWidth * 0.5f + margin, rows * stepHeight + 2.0f + margin, 2.0f);
    room.setBounds(minB, maxB);
}

void Application::run() {

    lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {

        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        // --- obrada inputa ---
        processInput(deltaTime);
        personManager->update((float)deltaTime);
        if (screen.isPlaying()) screen.update((float)deltaTime);

        // --- čišćenje ekrana ---
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "GL Error: " << err << std::hex << err << std::endl;
        }

        // --- overlay ---
        if (overlayInitialized) {
            glDisable(GL_DEPTH_TEST); // overlay uvek na vrhu
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glm::mat4 proj = glm::ortho(0.0f, float(width), 0.0f, float(height));

            glUseProgram(overlayShader);
            glUniformMatrix4fv(glGetUniformLocation(overlayShader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

            DrawOverlayRectangle(overlayShader, overlayVAO);

            glDisable(GL_BLEND);
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
        }

        // --- priprema view/projection za 3D scenu ---
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);

        glUseProgram(seatShader);
        glUniformMatrix4fv(glGetUniformLocation(seatShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(seatShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // --- crtanje sedista i ljudi ---
        personManager->renderScene(seatShader);

        glUseProgram(seatShader);
        glUniformMatrix4fv(glGetUniformLocation(seatShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(seatShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // nacrtaj prostoriju (pod, zidovi, plafon)
        room.render(seatShader, view, projection);
        {
            // dimenzije i pozicija platna unutar sobe (prilagodi ako treba)
            glm::vec3 screenSize((room.maxB.x - room.minB.x) * 0.7f, (room.maxB.y - room.minB.y) * 0.5f, 1.0f);
            glm::vec3 screenCenter((room.minB.x + room.maxB.x) * 0.5f,
                room.minB.y + screenSize.y * 0.5f + 0.5f,
                room.minB.z + 0.001f); // malo u front zida
            screen.render(seatShader, view, projection, screenCenter, screenSize);
        }
        

        // --- crtanje sedista i ljudi ---
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        personManager->renderScene(seatShader);

        // --- crtanje stepenica ---
        float spacingZ = 1.0f;
        float stepHeight = 0.3f;
        float startX = -((10 - 1) * 1.0f) / 2.0f;
        float endX = ((10 - 1) * 1.0f) / 2.0f;

        for (int r = 1; r <= 5; ++r) {
            drawSteps(startX, endX, r, stepHeight, spacingZ, view, projection);
        }

        // --- swap buffer-a i poll events ---
        glfwSwapBuffers(window);
        glfwPollEvents();

        // --- limit FPS ---
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

    // u Application::processInput
    static bool spacePressed = false;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
        if (screen.isPlaying()) {
            screen.stop();
        }
        else {
            screen.play();
        }
        personManager->spawnPeople();
        spacePressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        spacePressed = false;
    }

    static bool keyPressed[10] = { false };

    for (int i = 1; i <= 9; i++) {
        int key = GLFW_KEY_0 + i;

        // u Application::processInput, zameni deo za SPACE sa ovim
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
            // toggle play/stop
            if (screen.isPlaying()) {
                screen.stop();
            }
            else {
                personManager->spawnPeople(); // ostavi ako želiš da spawn ljudi ide pri startu
                screen.play();
            }
            spacePressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            spacePressed = false;
        }
    }

    camera.processKeyboard(forward, right, (float)deltaTime);

    camera.clampToBounds();
    camera.clampToSeats(seats3D);
    camera.clampToRoom(room.minB, room.maxB);

}

void Application::initSeats() {
    seats3D.clear();

    int rows = 5;
    int cols = 10;
    float spacingX = 1.0f;
    float spacingZ = 1.0f;
    float stepHeight = 0.3f;

    // VISINA sedišta (ako koristiš Seat3D::size, koristi ga)
    float seatHeight = !seats3D.empty() ? seats3D[0].size.y : 0.3f;
    float seatHalf = seatHeight * 0.5f;

    float startX = -((cols - 1) * spacingX) / 2.0f;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            glm::vec3 pos(
                startX + c * spacingX,
                r * stepHeight + seatHalf, // PODIGNUTO da dno kocke bude na stepenu
                -r * spacingZ - 5.0f
            );
            seats3D.emplace_back(pos);
        }
    }
}
void Application::drawSteps(float xStart, float xEnd, int rowIndex, float stepHeight, float spacingZ,
    const glm::mat4& view, const glm::mat4& projection)
{
    float yPrev = rowIndex * stepHeight + 0.5f;        // dno sedišta prethodnog reda
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
    glUniform1i(glGetUniformLocation(seatShader, "useTexture"), 0);
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

unsigned int Application::LoadTexture(const char* path) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return 0;
    }
    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texture;
}

glm::vec3 Application::screenPointToWorldRay(double mouseX, double mouseY, const glm::mat4 & view, const glm::mat4 & projection) {
    int w = width, h = height;
    // NDC
    float x = (2.0f * (float)mouseX) / w - 1.0f;
    float y = 1.0f - (2.0f * (float)mouseY) / h;
    glm::vec4 rayClip(x, y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    glm::vec4 rayWorld4 = glm::inverse(view) * rayEye;
    glm::vec3 rayWorld = glm::normalize(glm::vec3(rayWorld4));
    return rayWorld;
}

void Application::onMouseClick(double mouseX, double mouseY) {
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);
    glm::vec3 rayDir = screenPointToWorldRay(mouseX, mouseY, view, projection);
    glm::vec3 rayOrigin = camera.position;

    // testiraj sve sedece
    for (int r = 0; r < seatService.getNumRows(); ++r) {
        for (int c = 0; c < seatService.getNumCols(); ++c) {
            Seat* s = seatService.getSeat(r, c);
            glm::vec3 seatPos = personManager->seatToWorld(r, c); // make seatToWorld public, or compute same
            glm::vec3 half = seats3D[0].size * 0.5f;
            glm::vec3 min = seatPos - half;
            glm::vec3 max = seatPos + half;
            float tmin, tmax;
            // ray-AABB intersection (slight utility) - implement standard slab test
            if (rayIntersectsAABB(rayOrigin, rayDir, min, max)) {
                // rezerviši / kupi sedište
                seatService.markBought(s); // ili odgovarajuća metoda
                personManager->spawnPeople(); // osveži ljude ili šta treba
                return;
            }
        }
    }
}

bool Application::rayIntersectsAABB(const glm::vec3& rayOrig, const glm::vec3& rayDir,
    const glm::vec3& minB, const glm::vec3& maxB)
{
    const float EPS = 1e-8f;
    float tmin = (minB.x - rayOrig.x) / (fabs(rayDir.x) > EPS ? rayDir.x : (rayDir.x < 0 ? -EPS : EPS));
    float tmax = (maxB.x - rayOrig.x) / (fabs(rayDir.x) > EPS ? rayDir.x : (rayDir.x < 0 ? -EPS : EPS));
    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (minB.y - rayOrig.y) / (fabs(rayDir.y) > EPS ? rayDir.y : (rayDir.y < 0 ? -EPS : EPS));
    float tymax = (maxB.y - rayOrig.y) / (fabs(rayDir.y) > EPS ? rayDir.y : (rayDir.y < 0 ? -EPS : EPS));
    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax)) return false;

    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    float tzmin = (minB.z - rayOrig.z) / (fabs(rayDir.z) > EPS ? rayDir.z : (rayDir.z < 0 ? -EPS : EPS));
    float tzmax = (maxB.z - rayOrig.z) / (fabs(rayDir.z) > EPS ? rayDir.z : (rayDir.z < 0 ? -EPS : EPS));
    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax)) return false;

    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    return tmax >= 0.0f;
}

glm::vec3 Application::seatToWorldLocal(int row, int col) {
    float spacingX = 1.0f;
    float spacingZ = 1.0f;
    float stepHeight = 0.3f;
    int cols = seatService.getNumCols();
    float startX = -((cols - 1) * spacingX) / 2.0f;
    float seatHeight = !seats3D.empty() ? seats3D[0].size.y : 0.3f;
    float seatHalf = seatHeight * 0.5f;
    return glm::vec3(
        startX + col * spacingX,
        row * stepHeight + seatHalf,
        -row * spacingZ - 5.0f
    );
}