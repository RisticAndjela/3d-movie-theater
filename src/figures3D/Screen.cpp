#include "Screen.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include "stb_image.h" // DO NOT define STB_IMAGE_IMPLEMENTATION here if it's defined elsewhere

// Destructor: free textures and GL buffers
Screen::~Screen() {
    for (auto t : frames) {
        if (t) glDeleteTextures(1, &t);
    }
    frames.clear();

    if (quadVAO) {
        glDeleteVertexArrays(1, &quadVAO);
        quadVAO = 0;
    }
    if (quadVBO) {
        glDeleteBuffers(1, &quadVBO);
        quadVBO = 0;
    }
}

void Screen::ensureQuadInitialized() const {
    if (quadVAO != 0) return;

    // quad: pos.x, pos.y, pos.z, u, v (two triangles)
    const float quad[] = {
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f,

         0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // uv
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

bool Screen::loadFramesFromFolder(const std::string& folderPath) {
    frames.clear();

    namespace fs = std::filesystem;
    fs::path dir(folderPath);

    if (!fs::exists(dir) || !fs::is_directory(dir)) {
        std::cerr << "Screen::loadFramesFromFolder: folder not found: " << folderPath << std::endl;
        return false;
    }

    // collect png/jpg files
    std::vector<fs::path> files;
    for (auto& e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        auto ext = e.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
            files.push_back(e.path());
        }
    }

    if (files.empty()) {
        std::cerr << "Screen::loadFramesFromFolder: no image files in " << folderPath << std::endl;
        return false;
    }

    // sort by filename so frames are in order
    std::sort(files.begin(), files.end(), [](const fs::path& a, const fs::path& b) {
        return a.filename().string() < b.filename().string();
        });

    // load each file into GL texture
    for (auto& p : files) {
        int w = 0, h = 0, n = 0;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(p.string().c_str(), &w, &h, &n, 0);
        if (!data) {
            std::cerr << "Screen::loadFramesFromFolder: failed to load " << p << std::endl;
            continue;
        }

        GLenum format = GL_RGB;
        if (n == 1) format = GL_RED;
        else if (n == 3) format = GL_RGB;
        else if (n == 4) format = GL_RGBA;

        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // use clamp to edge for screen frames to avoid bleeding and linear filtering for smooth frames
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        frames.push_back(tex);
    }

    if (frames.empty()) {
        std::cerr << "Screen::loadFramesFromFolder: no frames loaded successfully from " << folderPath << std::endl;
        return false;
    }

    // reset animation
    acc = 0.0f;
    current = 0;
    return true;
}
void Screen::play() {
    if (frames.empty()) return;
    playing = true;
    acc = 0.0f;
    current = 0;
    std::cout << "Screen::play() called, frames=" << frames.size() << "\n";
}

void Screen::stop() {
    playing = false;
    acc = 0.0f;
    current = 0;
}

void Screen::update(float dt) {
    if (!playing || frames.empty()) return; // only advance when playing
    acc += dt;
    while (acc >= frameDuration) {
        acc -= frameDuration;
        current = (current + 1) % frames.size();
    }
}

void Screen::render(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection,
    const glm::vec3& center, const glm::vec3& size) const
{
    ensureQuadInitialized();
    glUseProgram(shaderProgram);

    // set view/projection/model if shader expects them (Application may already set them)
    GLint locView = glGetUniformLocation(shaderProgram, "view");
    if (locView >= 0) glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(view));
    GLint locProj = glGetUniformLocation(shaderProgram, "projection");
    if (locProj >= 0) glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 model = glm::translate(glm::mat4(1.0f), center);
    model = glm::scale(model, size);
    GLint locModel = glGetUniformLocation(shaderProgram, "model");
    if (locModel >= 0) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));

    if (playing && !frames.empty()) {
        // render current frame texture
        GLint locColor = glGetUniformLocation(shaderProgram, "color");
        if (locColor >= 0) glUniform3f(locColor, 1.0f, 1.0f, 1.0f);

        GLint locUseTex = glGetUniformLocation(shaderProgram, "useTexture");
        if (locUseTex >= 0) glUniform1i(locUseTex, 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frames[current]);
        GLint locSampler = glGetUniformLocation(shaderProgram, "seatTex");
        if (locSampler >= 0) glUniform1i(locSampler, 0);
    }
    else {
        // not playing -> white screen (no texture)
        GLint locColor = glGetUniformLocation(shaderProgram, "color");
        if (locColor >= 0) glUniform3f(locColor, 1.0f, 1.0f, 1.0f);
        GLint locUseTex = glGetUniformLocation(shaderProgram, "useTexture");
        if (locUseTex >= 0) glUniform1i(locUseTex, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // optional: restore useTexture = 0
    GLint locUseTex = glGetUniformLocation(shaderProgram, "useTexture");
    if (locUseTex >= 0) glUniform1i(locUseTex, 0);
}