#pragma once

#include "shader.h"
#include "vao.h"

#include "GLFW/glfw3.h"

typedef enum {
    TYPE_TRIANGLE,
    TYPE_SQUARE,
    TYPE_TWO_TRIANGLES,
    TYPE_SHADER_TRIANGLE,
    TYPE_LEN,
} Type;

typedef struct {
    GLFWwindow *window;
    VAO         vaos[MAX_VAOS];
    size_t      vao_len;
    Shader      shaders[MAX_SHADERS];
    size_t      shader_len;
    Type        type;
} Program;

bool program_init(Program *self, Type type);
void program_deinit(Program *self);
void program_run(Program *self);
// Add a VAO that is managed by `Program`.
VAO *program_add_vao(Program *self);
// Add a shader object that is managed by `Program`.
// The input shaders are not deleted by this function.
Shader *
program_add_shader(Program *self, GLuint vert_shader, GLuint frag_shader);

Type parse_args(int argc, char **argv);
void usage(const char *prog);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

typedef void (*SetupF)(Program *prog);
typedef void (*DrawF)(const Program *prog);
typedef void (*InputF)(Program *window);

extern const char *prog_str[TYPE_LEN];
extern SetupF      prog_setup[TYPE_LEN];
extern DrawF       prog_draw[TYPE_LEN];
extern InputF      prog_input[TYPE_LEN];

void setup(Program *prog);
void draw(const Program *prog);
void input(Program *window);
