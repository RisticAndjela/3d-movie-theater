cmake_minimum_required(VERSION 3.16)
project(Cinema3D)

set(CMAKE_CXX_STANDARD 17)

# Glavni izvorni fajlovi
add_executable(Cinema3D
    src/main.cpp
    src/Application.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Renderer.cpp
    src/HUD.cpp
    external/glad/src/glad.c
)

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
add_subdirectory(external/glfw)

# Include folders
include_directories(
    include                  
    external                 # za stb_image.h i font
    external/glad/include    # GLAD headeri
    external/glfw/include    # GLFW headeri
    external/glm
)

# Link biblioteke
target_link_libraries(Cinema3D
    glfw
    OpenGL::GL
)
