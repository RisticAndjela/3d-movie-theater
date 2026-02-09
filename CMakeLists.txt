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
    
    external/glad/src/glad.c
)

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
add_subdirectory(external/glfw)

# FreeType
set(FREETYPE_INCLUDE_DIR "external/freetype/include")

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
    ${FREETYPE_LIBRARY}
)

# Kopiranje shader fajlova u build folder
set(SHADER_SRC_DIR ${CMAKE_SOURCE_DIR}/assets/shaders)
set(SHADER_DST_DIR ${CMAKE_BINARY_DIR}/assets/shaders)

file(MAKE_DIRECTORY ${SHADER_DST_DIR})
file(GLOB SHADER_FILES "${SHADER_SRC_DIR}/*.vs" "${SHADER_SRC_DIR}/*.fs")

foreach(shader ${SHADER_FILES})
    add_custom_command(TARGET Cinema3D POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${shader} ${SHADER_DST_DIR}
    )
endforeach()
