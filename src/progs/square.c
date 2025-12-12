#include "square.h"
#include "prog.h"
#include "shader.h"
#include "vao.h"

include_file(shaders_beginning_frag);
include_file(shaders_beginning_vert);

static VAO    *vao;
static Shader *shader;

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

    vao = program_add_vao(prog);
    vao_add_vbo(vao, vertices, sizeof(vertices), GL_STATIC_DRAW);
    vao_vertex_attrib_ptr(vao, 0, 3, 0, 3 * sizeof(float), GL_FLOAT, false);
    vao_set_ebo(vao, indices, sizeof(indices), GL_STATIC_DRAW);

    GLuint vert_shader =
        compile_shader(GL_VERTEX_SHADER, shaders_beginning_vert);
    GLuint frag_shader =
        compile_shader(GL_FRAGMENT_SHADER, shaders_beginning_frag);
    shader = program_add_shader(prog, vert_shader, frag_shader);
    delete_shader(vert_shader);
    delete_shader(frag_shader);
}

void square_draw(const Program *prog) {
    (void) prog;

    shader_bind(shader);
    vao_bind(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    vao_unbind();
    shader_unbind();
}
