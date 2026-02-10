#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

class Screen {
public:
    Screen() = default;
    ~Screen();

    bool loadFramesFromFolder(const std::string& folderPath);
    void update(float dt);
    void render(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection,
        const glm::vec3& center, const glm::vec3& size) const;

    bool hasFrames() const { return !frames.empty(); }
    void setFrameDuration(float seconds) { frameDuration = seconds; }

    // playback control
    void play();      // start / resume playback (resets to first frame)
    void stop();      // stop playback
    bool isPlaying() const { return playing; }

    mutable unsigned int quadVAO = 0;
    mutable unsigned int quadVBO = 0;

private:
    std::vector<unsigned int> frames;
    float frameDuration = 1.0f / 24.0f;
    float acc = 0.0f;
    size_t current = 0;
    bool playing = false;

    void ensureQuadInitialized() const;
};