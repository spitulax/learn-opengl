#include "utils.h"

#include "prog.h"
#include "progs.h"
#include <stdio.h>
#include <stdlib.h>

#define PROG(name)                                                             \
    void name##_start(Program *prog);                                          \
    void name##_draw(Program *prog);                                           \
    void name##_input(Program *prog);                                          \
    void name##_end(Program *prog);
PROGS
#undef PROG

#define PROG(name) [TYPE_##name] = #name,
const char *prog_str[TYPE_LEN] = { PROGS };
#undef PROG

#define PROG(name) [TYPE_##name] = name##_start,
ProgF prog_start[TYPE_LEN] = { PROGS };
#undef PROG

#define PROG(name) [TYPE_##name] = name##_draw,
ProgF prog_draw[TYPE_LEN] = { PROGS };
#undef PROG

#define PROG(name) [TYPE_##name] = name##_input,
ProgF prog_input[TYPE_LEN] = { PROGS };
#undef PROG

#define PROG(name) [TYPE_##name] = name##_end,
ProgF prog_end[TYPE_LEN] = { PROGS };
#undef PROG

bool program_init(Program *self, Type type) {
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

    self->type = type;

defer:
    if (!res) {
        self = NULL;
    }

    return res;
}

void program_deinit(Program *self) {
    if (self) {
        end(self);
    }

    glfwTerminate();

    zero(self);
}

void program_run(Program *self) {
    start(self);

    while (!glfwWindowShouldClose(self->window)) {
        input(self);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw(self);

        glfwSwapBuffers(self->window);
        glfwPollEvents();
    }
}

Type parse_args(int argc, char **argv) {
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
            if (scmp(subcommand, prog_str[i])) {
                return (Type) i;
            }
        }
    }

    eprintf("Invalid mini-program `%s`\n", subcommand);
    usage(prog);
    exit(1);
}

void usage(const char *prog) {
    eprintf("Usage: %s <mini-program>\n", prog);
    eprintf("\nMini-programs:\n");
    for (size_t i = 0; i < TYPE_LEN; ++i) {
        eprintf("- %s\n", prog_str[i]);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    (void) window;

    glViewport(0, 0, width, height);
}

void start(Program *prog) {
    prog_start[prog->type](prog);
}

void draw(Program *prog) {
    prog_draw[prog->type](prog);
}

void input(Program *prog) {
    if (glfwGetKey(prog->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(prog->window, true);
    }

    prog_input[prog->type](prog);
}

void end(Program *prog) {
    prog_end[prog->type](prog);
}
