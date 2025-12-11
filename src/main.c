#include <glad/glad.h>
#include <stdbool.h>
#include <stdio.h>

#include <GLFW/glfw3.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

#define return_defer(value)                                                    \
    {                                                                          \
        res = (value);                                                         \
        goto defer;                                                            \
    }

static void
framebuffer_size_callback(GLFWwindow *window, int width, int height);

static void process_input(GLFWwindow *window);

int main(int argc, char *argv[]) {
    int res = 0;

    (void) argc;
    (void) argv;

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

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

defer:
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
