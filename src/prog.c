#include "prog.h"
#include "progs/shader_triangle.h"
#include "progs/square.h"
#include "progs/texture.h"
#include "progs/triangle.h"
#include "progs/two_triangles.h"
#include "shader.h"
#include "utils.h"
#include "vao.h"

const char *prog_str[TYPE_LEN] = {
    [TYPE_TRIANGLE]        = "triangle",
    [TYPE_SQUARE]          = "square",
    [TYPE_TWO_TRIANGLES]   = "two_triangles",
    [TYPE_SHADER_TRIANGLE] = "shader_triangle",
    [TYPE_TEXTURE]         = "texture",
};

SetupF prog_setup[TYPE_LEN] = {
    [TYPE_TRIANGLE]        = triangle_setup,
    [TYPE_SQUARE]          = square_setup,
    [TYPE_TWO_TRIANGLES]   = two_triangles_setup,
    [TYPE_SHADER_TRIANGLE] = shader_triangle_setup,
    [TYPE_TEXTURE]         = texture_setup,
};

DrawF prog_draw[TYPE_LEN] = {
    [TYPE_TRIANGLE]        = triangle_draw,
    [TYPE_SQUARE]          = square_draw,
    [TYPE_TWO_TRIANGLES]   = two_triangles_draw,
    [TYPE_SHADER_TRIANGLE] = shader_triangle_draw,
    [TYPE_TEXTURE]         = texture_draw,
};

InputF prog_input[TYPE_LEN] = {
    [TYPE_TRIANGLE]        = NULL,
    [TYPE_SQUARE]          = NULL,
    [TYPE_TWO_TRIANGLES]   = NULL,
    [TYPE_SHADER_TRIANGLE] = NULL,
};

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
    return res;
}

void program_deinit(Program *self) {
    (void) self;

    for (size_t i = 0; i < self->vao_len; ++i) {
        vao_deinit(&self->vaos[i]);
    }
    for (size_t i = 0; i < self->shader_len; ++i) {
        shader_deinit(&self->shaders[i]);
    }
    glfwTerminate();

    zero(self);
}

void program_run(Program *self) {
    while (!glfwWindowShouldClose(self->window)) {
        input(self);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw(self);

        glfwSwapBuffers(self->window);
        glfwPollEvents();
    }
}

VAO *program_add_vao(Program *self) {
    VAO *vao = self->vaos + self->vao_len++;
    vao_init(vao);
    return vao;
}

Shader *
program_add_shader(Program *self, GLuint vert_shader, GLuint frag_shader) {
    Shader *shader = self->shaders + self->shader_len++;
    shader_init(shader, vert_shader, frag_shader);
    return shader;
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

    // TODO: Fix usage
    eprintf("Invalid type `%s`\n", subcommand);
    usage(prog);
    exit(1);
}

void usage(const char *prog) {
    eprintf("Usage: %s <", prog);
    for (size_t i = 0; i < TYPE_LEN; ++i) {
        if (i > 0) {
            eprintf(", ");
        }
        eprintf("%s", prog_str[i]);
    }
    eprintf(">\n");
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    (void) window;

    glViewport(0, 0, width, height);
}

void setup(Program *prog) {
    const SetupF fn = prog_setup[prog->type];
    if (fn) {
        fn(prog);
    }
}

void draw(const Program *prog) {
    const DrawF fn = prog_draw[prog->type];
    if (fn) {
        fn(prog);
    }
}

void input(Program *prog) {
    if (glfwGetKey(prog->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(prog->window, true);
    }

    const InputF fn = prog_input[prog->type];
    if (fn) {
        fn(prog);
    }
}
