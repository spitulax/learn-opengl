#include <glad/glad.h>
#include <stdbool.h>
#include <stdio.h>

#include <GLFW/glfw3.h>

#define WINDOW_WIDTH         800
#define WINDOW_HEIGHT        600
#define SHADER_INFO_LOG_SIZE 1024

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

#define return_defer(value)                                                    \
    {                                                                          \
        res = (value);                                                         \
        goto defer;                                                            \
    }

#define include_file(name)                                                     \
    extern unsigned char ___resources_##name[];                                \
    extern unsigned int  ___resources_##name##_len

#define compileShader(type, file_name)                                         \
    compileShaderManual(type,                                                  \
                        #file_name,                                            \
                        ___resources_##file_name,                              \
                        ___resources_##file_name##_len)

static void
framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void   process_input(GLFWwindow *window);
static GLuint compileShaderManual(GLenum               type,
                                  const char          *file_name,
                                  const unsigned char *string,
                                  unsigned int         len);

include_file(vert_glsl);
include_file(frag_glsl);

int main(int argc, char *argv[]) {
    int res = 0;

    (void) argc;
    (void) argv;

    GLuint vao = 0, vbo = 0;

    printf("Hello, C! learn-opengl version %s\n", PROG_VERSION);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(
        WINDOW_WIDTH, WINDOW_HEIGHT, "Learn Opengl", NULL, NULL);
    if (!window) {
        eprintf("GLFW: Failed to create window.\n");
        return_defer(1);
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        eprintf("Glad: Failed to load.\n");
        return_defer(1);
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    float vertices[] = {
        0.0f,  0.5f,  0.0f,    //
        -0.5f, -0.5f, 0.0f,    //
        0.5f,  -0.5f, 0.0f,    //
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint vert_shader = compileShader(GL_VERTEX_SHADER, vert_glsl);
    GLuint frag_shader = compileShader(GL_FRAGMENT_SHADER, frag_glsl);
    GLuint shader_prog = glCreateProgram();
    glAttachShader(shader_prog, vert_shader);
    glAttachShader(shader_prog, frag_shader);
    glLinkProgram(shader_prog);
    int  success;
    char info_log[SHADER_INFO_LOG_SIZE];
    glGetProgramiv(shader_prog, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_prog, SHADER_INFO_LOG_SIZE, NULL, info_log);
        eprintf("Failed to link shaders: %s\n", info_log);
    }
    glUseProgram(shader_prog);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_prog);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

defer:
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
    }

    if (vao != 0) {
        glDeleteBuffers(1, &vao);
    }

    glfwTerminate();

    return res;
}

static void
framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    (void) window;

    glViewport(0, 0, width, height);
}

static void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

static GLuint compileShaderManual(GLenum               type,
                                  const char          *file_name,
                                  const unsigned char *string,
                                  unsigned int         len) {
    GLuint shader = glCreateShader(type);
    glShaderSource(
        shader, 1, (const GLchar *const *) &string, (const GLint *) &len);
    glCompileShader(shader);

    GLint success;
    char  info_log[SHADER_INFO_LOG_SIZE];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, SHADER_INFO_LOG_SIZE, NULL, info_log);
        eprintf("Failed to compile shader %s: %s\n", file_name, info_log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}
