#include "triangle.h"
#include "files.h"

void triangle_setup(Program *prog) {
    float vertices[] = {
        0.0f,  0.5f,  0.0f,    //
        -0.5f, -0.5f, 0.0f,    //
        0.5f,  -0.5f, 0.0f,    //
    };

    vao_add_vbo(&prog->vao, vertices, sizeof(vertices), GL_STATIC_DRAW);
    vao_vertex_attrib_ptr(
        &prog->vao, 0, 0, 3, GL_FLOAT, false, 3 * sizeof(vertices[0]));

    GLuint vert_shader = compile_shader(GL_VERTEX_SHADER, vert_glsl);
    GLuint frag_shader = compile_shader(GL_FRAGMENT_SHADER, frag_glsl);
    program_init_shader(prog, vert_shader, frag_shader);
}

void triangle_draw(const Program *prog) {
    glUseProgram(prog->shader_prog);
    glBindVertexArray(prog->vao.name);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
