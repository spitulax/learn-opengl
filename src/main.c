#include <assert.h>
#include <glad/glad.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GLFW/glfw3.h>

/** MACROS **/

#define WINDOW_WIDTH         800
#define WINDOW_HEIGHT        600
#define SHADER_INFO_LOG_SIZE 1024
#define MAX_VBOS             16

#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#define zero(ptr)    memset((ptr), 0, sizeof(*(ptr)))
#define panic(msg)   assert(0 && (msg))
#define scmp(a, b)   (strcmp((a), (b)) == 0)


#define return_defer(value)                                                    \
    {                                                                          \
        res = (value);                                                         \
        goto defer;                                                            \
    }

#define include_file(name)                                                     \
    extern unsigned char ___resources_##name[];                                \
    extern unsigned int  ___resources_##name##_len

#define compile_shader(type, file_name)                                        \
    compile_shader_manual(type,                                                \
                          #file_name,                                          \
                          ___resources_##file_name,                            \
                          ___resources_##file_name##_len)

/** TYPES **/

typedef GLsizeiptr ssize_t;

typedef struct {
    GLuint name;
    GLuint vbos[MAX_VBOS];
    size_t vbo_len;
} VAO;

typedef enum {
    TYPE_TRIANGLE,
    TYPE_LEN,
} Type;

const char *type_str[TYPE_LEN] = {
    [TYPE_TRIANGLE] = "triangle",
};

typedef struct {
    GLFWwindow *window;
    VAO         vao;
    GLuint      shader_prog;
    Type        type;
} Program;

/** PROTOTYPES **/

/* Utils */
static void
framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void     process_input(GLFWwindow *window);
static uint32_t compile_shader_manual(GLenum               type,
                                      const char          *file_name,
                                      const unsigned char *string,
                                      unsigned int         len);
static Type     parse_args(int argc, char **argv);
static void     usage(const char *prog);

/* Program */
static bool program_init(Program *self, Type type);
static void program_deinit(Program *self);
static void program_run(const Program *self);
static void
program_init_shader(Program *self, GLuint vert_shader, GLuint frag_shader);

/* VAO */
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

/* Rendering */
static void setup(Program *prog);
static void draw(const Program *prog);

/** FILES **/

include_file(vert_glsl);
include_file(frag_glsl);

/** FUNCTIONS **/

int main(int argc, char *argv[]) {
    int res = 0;

    Type type = parse_args(argc, argv);

    Program prog;
    if (!program_init(&prog, type)) {
        return_defer(1);
    }

    setup(&prog);

    program_run(&prog);

defer:
    program_deinit(&prog);

    return res;
}

/* Utils */

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

static Type parse_args(int argc, char **argv) {
    const char *prog = argv[0];

    if (argc != 2) {
        eprintf("Invalid arguments\n");
        usage(prog);
        exit(1);
    }

    const char *subcommand = argv[1];
    if (scmp(subcommand, "help") || scmp(subcommand, "--help") ||
        scmp(subcommand, "-h")) {
        usage(prog);
        exit(0);
    } else {
        for (size_t i = 0; i < TYPE_LEN; ++i) {
            if (scmp(subcommand, type_str[i])) {
                return (Type) i;
            }
        }
    }

    eprintf("Invalid type `%s`\n", subcommand);
    usage(prog);
    exit(1);
}

static void usage(const char *prog) {
    eprintf("Usage: %s <", prog);
    for (size_t i = 0; i < TYPE_LEN; ++i) {
        if (i > 0) {
            eprintf(", ");
        }
        eprintf("%s", type_str[i]);
    }
    eprintf(">\n");
}

/* Program */

static bool program_init(Program *self, Type type) {
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

    self->type = type;

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

static void program_run(const Program *self) {
    while (!glfwWindowShouldClose(self->window)) {
        process_input(self->window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw(self);

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

/* VAO */

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

/* Rendering */

static void setup(Program *prog) {
    switch (prog->type) {
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

static void draw(const Program *prog) {
    switch (prog->type) {
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
