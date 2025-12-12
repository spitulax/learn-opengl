#pragma once

// Do not remove
#include "utils.h"

#include "progs.h"

#define PROG(name) TYPE_##name,

typedef enum {
    PROGS TYPE_LEN,
} Type;

#undef PROG

typedef struct {
    GLFWwindow *window;
    Type        type;
} Program;

bool program_init(Program *self, Type type);
void program_deinit(Program *self);
void program_run(Program *self);

Type parse_args(int argc, char **argv);
void usage(const char *prog);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

typedef void (*ProgF)(Program *prog);

extern const char *prog_str[TYPE_LEN];
extern ProgF       prog_start[TYPE_LEN];
extern ProgF       prog_draw[TYPE_LEN];
extern ProgF       prog_input[TYPE_LEN];
extern ProgF       prog_end[TYPE_LEN];

void start(Program *prog);
void draw(Program *prog);
void input(Program *prog);
void end(Program *prog);
