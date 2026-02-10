cmake_minimum_required(VERSION 3.16)
project(Cinema3D)

set(CMAKE_CXX_STANDARD 17)

# Build type ako nije definisan
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Glavni izvori
add_executable(Cinema3D
    src/main.cpp
    src/Application.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Renderer.cpp
    src/figures3D/Seat3D.cpp 
    src/figures3D/Screen.cpp
    src/figures3D/Room3D.cpp
    external/glad/src/glad.c
 "assets/models/OverlayRectangle.h" "assets/models/OverlayRectangle.cpp"   "src/service/SeatService.cpp" "src/model/Person.h" "src/model/Seat.h" "src/figures3D/Seat3D.h" "src/service/PersonManager.h" "src/service/PersonManager.cpp" "src/service/SeatService.h" "src/figures3D/Room3D.h" "src/figures3D/Room3D.cpp" "src/figures3D/Screen.h" "src/figures3D/Screen.cpp")

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
add_subdirectory(external/glfw)

# Include folders
target_include_directories(Cinema3D PRIVATE
    include
    external/glad/include
    external/glm
    external/glfw/include
    external
)

# Link libraries
target_link_libraries(Cinema3D PRIVATE
    glfw
    OpenGL::GL
)
