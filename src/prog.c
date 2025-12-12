#include "prog.h"
#include "progs/square.h"
#include "progs/triangle.h"
#include "utils.h"

const char *prog_str[TYPE_LEN] = {
    [TYPE_TRIANGLE] = "triangle",
    [TYPE_SQUARE]   = "square",
};

SetupF prog_setup[TYPE_LEN] = {
    [TYPE_TRIANGLE] = triangle_setup,
    [TYPE_SQUARE]   = square_setup,
};

DrawF prog_draw[TYPE_LEN] = {
    [TYPE_TRIANGLE] = triangle_draw,
    [TYPE_SQUARE]   = square_draw,
};

InputF prog_input[TYPE_LEN] = {
    [TYPE_TRIANGLE] = NULL,
    [TYPE_SQUARE]   = NULL,
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

    vao_init(&self->vao);

    self->type = type;

defer:
    return res;
}

void program_deinit(Program *self) {
    (void) self;

    vao_deinit(&self->vao);
    glDeleteProgram(self->shader_prog);
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

void program_init_shader(Program *self,
                         GLuint   vert_shader,
                         GLuint   frag_shader) {
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
