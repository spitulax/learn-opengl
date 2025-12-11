#include <assert.h>
#include <glad/glad.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <GLFW/glfw3.h>

#define WINDOW_WIDTH         800
#define WINDOW_HEIGHT        600
#define SHADER_INFO_LOG_SIZE 1024
#define MAX_VBOS             16

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

#define return_defer(value)                                                    \
    {                                                                          \
        res = (value);                                                         \
        goto defer;                                                            \
    }

#define zero(ptr) memset(ptr, 0, sizeof(*ptr))

#define panic(msg) assert(0 && msg)

#define include_file(name)                                                     \
    extern unsigned char ___resources_##name[];                                \
    extern unsigned int  ___resources_##name##_len

#define compile_shader(type, file_name)                                        \
    compile_shader_manual(type,                                                \
                          #file_name,                                          \
                          ___resources_##file_name,                            \
                          ___resources_##file_name##_len)

typedef GLsizeiptr ssize_t;

typedef struct {
    GLuint name;
    GLuint vbos[MAX_VBOS];
    size_t vbo_len;
} VAO;

typedef struct {
    GLFWwindow *window;
    VAO         vao;
    GLuint      shader_prog;
} Program;

typedef enum {
    TYPE_TRIANGLE,
} Type;

static void
framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void     process_input(GLFWwindow *window);
static uint32_t compile_shader_manual(GLenum               type,
                                      const char          *file_name,
                                      const unsigned char *string,
                                      unsigned int         len);
static bool     program_init(Program *self);
static void     program_deinit(Program *self);
static void     program_run(const Program *self, Type type);
static void
program_init_shader(Program *self, GLuint vert_shader, GLuint frag_shader);
static void vao_init(VAO *self);
static void vao_deinit(VAO *self);
static void vao_add_vbo(
    VAO *self, GLenum type, const void *data, ssize_t size, GLenum usage);
static void vao_vertex_attrib_ptr(VAO     *self,
                                  GLuint   vbo,
                                  uint32_t index,
                                  int32_t  size,
                                  GLenum   type,
                                  bool     normalized,
                                  int32_t  stride);
static void setup(Program *prog, Type type);
static void draw(const Program *prog, Type type);

include_file(vert_glsl);
include_file(frag_glsl);

int main(int argc, char *argv[]) {
    int res = 0;

    (void) argc;
    (void) argv;

    Program prog;
    if (!program_init(&prog)) {
        return_defer(1);
    }

    Type type = TYPE_TRIANGLE;
    setup(&prog, type);

    program_run(&prog, type);

defer:
    program_deinit(&prog);

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

static uint32_t compile_shader_manual(GLenum               type,
                                      const char          *file_name,
                                      const unsigned char *string,
                                      unsigned int         len) {
    GLuint shader = glCreateShader(type);
    glShaderSource(
        shader, 1, (const char *const *) &string, (const int32_t *) &len);
    glCompileShader(shader);

    int32_t success;
    char    info_log[SHADER_INFO_LOG_SIZE];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, SHADER_INFO_LOG_SIZE, NULL, info_log);
        eprintf("Failed to compile shader %s: %s\n", file_name, info_log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static bool program_init(Program *self) {
    bool res = true;

    zero(self);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    self->window = glfwCreateWindow(
        WINDOW_WIDTH, WINDOW_HEIGHT, "Learn Opengl", NULL, NULL);
    if (!self->window) {
        eprintf("GLFW: Failed to create window.\n");
        return_defer(false);
    }
    glfwMakeContextCurrent(self->window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        eprintf("Glad: Failed to load.\n");
        return_defer(false);
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetFramebufferSizeCallback(self->window, framebuffer_size_callback);

    vao_init(&self->vao);

defer:
    return res;
}

static void program_deinit(Program *self) {
    (void) self;

    vao_deinit(&self->vao);
    glDeleteProgram(self->shader_prog);
    glfwTerminate();

    zero(self);
}

static void program_run(const Program *self, Type type) {
    while (!glfwWindowShouldClose(self->window)) {
        process_input(self->window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw(self, type);

        glfwSwapBuffers(self->window);
        glfwPollEvents();
    }
}

static void
program_init_shader(Program *self, GLuint vert_shader, GLuint frag_shader) {
    self->shader_prog = glCreateProgram();
    glAttachShader(self->shader_prog, vert_shader);
    glAttachShader(self->shader_prog, frag_shader);
    glLinkProgram(self->shader_prog);

    int  success;
    char info_log[SHADER_INFO_LOG_SIZE];
    glGetProgramiv(self->shader_prog, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(
            self->shader_prog, SHADER_INFO_LOG_SIZE, NULL, info_log);
        eprintf("Failed to link shader: %s\n", info_log);
    }

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
}

static void vao_init(VAO *self) {
    zero(self);

    glGenVertexArrays(1, &self->name);
    // NOTE: Assuming there is only one VAO
    glBindVertexArray(self->name);
}

static void vao_deinit(VAO *self) {
    if (self->name != 0) {
        glDeleteVertexArrays(1, &self->name);
    }

    glDeleteBuffers(MAX_VBOS, self->vbos);

    zero(self);
}

static void vao_add_vbo(
    VAO *self, GLenum type, const void *data, GLsizeiptr size, GLenum usage) {
    GLuint *vbo = self->vbos + self->vbo_len++;
    glGenBuffers(1, vbo);
    glBindBuffer(type, *vbo);
    glBufferData(type, size, data, usage);
}

static void vao_vertex_attrib_ptr(VAO     *self,
                                  GLuint   vbo,
                                  uint32_t index,
                                  int32_t  size,
                                  GLenum   type,
                                  bool     normalized,
                                  int32_t  stride) {
    (void) self;
    glVertexAttribPointer(
        index, size, type, normalized, stride, (void *) (size_t) vbo);
    glEnableVertexAttribArray(index);
}

static void setup(Program *prog, Type type) {
    switch (type) {
        case TYPE_TRIANGLE: {
            float vertices[] = {
                0.0f,  0.5f,  0.0f,    //
                -0.5f, -0.5f, 0.0f,    //
                0.5f,  -0.5f, 0.0f,    //
            };

            vao_add_vbo(&prog->vao,
                        GL_ARRAY_BUFFER,
                        vertices,
                        sizeof(vertices),
                        GL_STATIC_DRAW);
            vao_vertex_attrib_ptr(
                &prog->vao, 0, 0, 3, GL_FLOAT, false, 3 * sizeof(vertices[0]));

            GLuint vert_shader = compile_shader(GL_VERTEX_SHADER, vert_glsl);
            GLuint frag_shader = compile_shader(GL_FRAGMENT_SHADER, frag_glsl);
            program_init_shader(prog, vert_shader, frag_shader);
        } break;
        default: {
            panic("Unimplemented type");
        }
    }
}

static void draw(const Program *prog, Type type) {
    switch (type) {
        case TYPE_TRIANGLE: {
            glUseProgram(prog->shader_prog);
            glBindVertexArray(prog->vao.name);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        } break;
        default: {
            panic("Unimplemented type");
        }
    }
}
