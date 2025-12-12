#include "triangle.h"
#include "prog.h"
#include "shader.h"
#include "vao.h"

include_file(shaders_beginning_frag);
include_file(shaders_beginning_vert);

static VAO    *vao;
static Shader *shader;

void triangle_setup(Program *prog) {
    float vertices[] = {
        0.0f,  0.5f,  0.0f,    //
        -0.5f, -0.5f, 0.0f,    //
        0.5f,  -0.5f, 0.0f,    //
    };

    vao = program_add_vao(prog);
    vao_add_vbo(vao, vertices, sizeof(vertices), GL_STATIC_DRAW);
    vao_vertex_attrib_ptr(
        vao, 0, 0, 3, GL_FLOAT, false, 3 * sizeof(vertices[0]));

    GLuint vert_shader =
        compile_shader(GL_VERTEX_SHADER, shaders_beginning_vert);
    GLuint frag_shader =
        compile_shader(GL_FRAGMENT_SHADER, shaders_beginning_frag);
    shader = program_add_shader(prog, vert_shader, frag_shader);
    delete_shader(vert_shader);
    delete_shader(frag_shader);
}

void triangle_draw(const Program *prog) {
    (void) prog;

    shader_bind(shader);
    vao_bind(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    vao_unbind();
    shader_unbind();
}
