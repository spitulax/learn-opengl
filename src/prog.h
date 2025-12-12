#pragma once

#include "vao.h"

#include "GLFW/glfw3.h"

typedef enum {
    TYPE_TRIANGLE,
    TYPE_SQUARE,
    TYPE_LEN,
} Type;

typedef struct {
    GLFWwindow *window;
    VAO         vao;
    GLuint      shader_prog;
    Type        type;
} Program;

bool program_init(Program *self, Type type);
void program_deinit(Program *self);
void program_run(Program *self);
void program_init_shader(Program *self, GLuint vert_shader, GLuint frag_shader);

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
