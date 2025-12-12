#include "render.h"
#include "render/square.h"
#include "render/triangle.h"
#include "utils.h"

void setup(Program *prog) {
    switch (prog->type) {
        case TYPE_TRIANGLE: {
            triangle_setup(prog);
        } break;
        case TYPE_SQUARE: {
            square_setup(prog);
        } break;
        default: {
            panic("Unimplemented type\n");
        }
    }
}

void draw(const Program *prog) {
    switch (prog->type) {
        case TYPE_TRIANGLE: {
            triangle_draw(prog);
        } break;
        case TYPE_SQUARE: {
            square_draw(prog);
        } break;
        default: {
            panic("Unimplemented type\n");
        }
    }
}

void process_input(Program *prog) {
    if (glfwGetKey(prog->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(prog->window, true);
    }
}
