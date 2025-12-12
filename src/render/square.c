#include "square.h"
#include "files.h"

void square_setup(Program *prog) {
    float vertices[] = {
        0.5f,  0.5f,  0.0f,    // top right
        0.5f,  -0.5f, 0.0f,    // bottom right
        -0.5f, -0.5f, 0.0f,    // bottom left
        -0.5f, 0.5f,  0.0f     // top left
    };

    uint32_t indices[] = {
        0, 1, 3,    // first triangle
        1, 2, 3,    // second triangle
    };

    vao_add_vbo(&prog->vao, vertices, sizeof(vertices), GL_STATIC_DRAW);
    vao_vertex_attrib_ptr(
        &prog->vao, 0, 0, 3, GL_FLOAT, false, 3 * sizeof(vertices[0]));

    vao_set_ebo(&prog->vao, indices, sizeof(indices), GL_STATIC_DRAW);

    GLuint vert_shader = compile_shader(GL_VERTEX_SHADER, vert_glsl);
    GLuint frag_shader = compile_shader(GL_FRAGMENT_SHADER, frag_glsl);
    program_init_shader(prog, vert_shader, frag_shader);
}

void square_draw(const Program *prog) {
    glUseProgram(prog->shader_prog);
    glBindVertexArray(prog->vao.name);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
