#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_ES2
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#if defined(GLFW_INCLUDE_ES2)
#   pragma comment(lib, "libEGL.dll.lib")
#   pragma comment(lib, "libGLESv2.dll.lib")
#else
#   pragma comment(lib, "opengl32.lib")
#endif

#include "EGL/egl.h"

int WindowWidth = 800;
int WindowHeight = 450;
const char* AppTitle = "OpenGLES2";

int rnd(int min, int max) {
    return min + (int)(rand() * (max - min + 1.0) / (1.0 + RAND_MAX));
}

void glfw_error_callback(int error, const char *message) {
    fprintf(stderr, "GLFW: %s\n", message);
}

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

GLuint loadShader(GLenum type, const char *shader_src) {
    GLuint shader = glCreateShader (type);
    glShaderSource (shader, 1, &shader_src, NULL);
    glCompileShader (shader);
    GLint compiled;
    glGetShaderiv (shader, GL_COMPILE_STATUS, &compiled);
    return shader;
}

int main() {
    if (glfwInit() == GL_FALSE) {
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ウィンドウの生成
    GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, AppTitle, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, glfw_key_callback); // キー入力コールバック
    glfwSetErrorCallback(glfw_error_callback); // エラーコールバック

    // OpenGLの処理対象にする
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    GLbyte vShaderStr[] =
        "attribute vec4 a_position;\n"
        "attribute vec2 a_texCoord;\n"
        "varying vec2 v_texCoord;\n"
        "void main() {\n"
        "gl_Position = a_position;\n"
        "v_texCoord = a_texCoord;\n"
        "}";
    GLbyte fShaderStr[] =
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "uniform sampler2D s_texture;\n"
        "void main() {\n"
        "gl_FragColor = texture2D(s_texture, v_texCoord);\n"
        "}\n";
    GLuint program = glCreateProgram(); // Handle to a program object
    GLuint vs = loadShader (GL_VERTEX_SHADER, (const char*)vShaderStr);
    GLuint fs = loadShader (GL_FRAGMENT_SHADER, (const char*)fShaderStr);
    glAttachShader(program, vs);
    glDeleteShader(vs);
    glAttachShader(program, fs);
    glDeleteShader(fs);
    
    glLinkProgram (program); // Link the program
    GLint mPositionLoc = glGetAttribLocation(program, "a_position"); // Attribute locations
    GLint mTexCoordLoc = glGetAttribLocation(program, "a_texCoord");
    GLint mSamplerLoc = glGetUniformLocation(program, "s_texture"); // Sampler location

    // Create Simple Texture 2D
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Use tightly packed data
    GLuint texture; // Generate a texture object
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // Bind the texture object
    const size_t width = 320; // Load the texture: 2x2 Image, 3 bytes per pixel (R, G, B)
    const size_t height = 180;
    GLubyte pixels[width * height * 3];
    for(int i = 0; i < width * height * 3; i++) {
        pixels[i] = rnd(0,255);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Set the filtering mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLuint mTexture = texture; // Texture handle
    
    
    GLfloat vertices[] = {
        -1.0f, 1.0f,  0.0f,  // Position 0
        0.0f,  0.0f,         // TexCoord 0
        -1.0f, -1.0f, 0.0f,  // Position 1
        0.0f,  1.0f,         // TexCoord 1
        1.0f,  -1.0f, 0.0f,  // Position 2
        1.0f,  1.0f,         // TexCoord 2
        1.0f,  1.0f,  0.0f,  // Position 3
        1.0f,  0.0f          // TexCoord 3
    };
    GLushort indices[] = {0, 1, 2, 0, 2, 3};

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPostEmptyEvent();

        glViewport(0, 0, WindowWidth * 2, WindowHeight * 2); // Set the viewport
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
        glUseProgram(program); // Use the program object
        glVertexAttribPointer(mPositionLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), vertices); // Load the vertex position
        glVertexAttribPointer(mTexCoordLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), vertices + 3); // Load the texture coordinate
        glEnableVertexAttribArray(mPositionLoc);
        glEnableVertexAttribArray(mTexCoordLoc);
        for(int i = 0; i < width * height * 3; i++) {
            pixels[i] = rnd(0,255);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        glActiveTexture(GL_TEXTURE0); // Bind the texture
        glBindTexture(GL_TEXTURE_2D, mTexture);
        glUniform1i(mSamplerLoc, 0); // Set the texture sampler to texture unit to 0
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
        
        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return -1;
}
