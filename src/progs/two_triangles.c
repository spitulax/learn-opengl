#include "prog.h"
#include "shader.h"
#include "vao.h"

include_file(shaders_beginning_frag);
include_file(shaders_beginning_vert);
include_file(shaders_two_triangles_second_frag);

static VAO    vao[2];
static Shader shader[2];

void two_triangles_start(Program *prog) {
    (void) prog;

    float t1_vertices[] = {
        -0.5f,  -0.5f, 0.0f,    //
        0.0f,   -0.5f, 0.0f,    //
        -0.25f, 0.5f,  0.0f,    //
    };

    float t2_vertices[] = {
        0.0f,  0.5f,  0.0f,    //
        0.5f,  0.5f,  0.0f,    //
        0.25f, -0.5f, 0.0f,    //
    };

    vao_init(&vao[0]);
    vao_add_vbo(&vao[0], t1_vertices, sizeof(t1_vertices), GL_STATIC_DRAW);
    vao_vertex_attrib_ptr(&vao[0], 0, 3, 0, 3 * sizeof(float), GL_FLOAT, false);

    vao_init(&vao[1]);
    vao_add_vbo(&vao[1], t2_vertices, sizeof(t2_vertices), GL_STATIC_DRAW);
    vao_vertex_attrib_ptr(&vao[1], 0, 3, 0, 3 * sizeof(float), GL_FLOAT, false);

    GLuint vert_shader =
        compile_shader(GL_VERTEX_SHADER, shaders_beginning_vert);
    GLuint t1_frag_shader =
        compile_shader(GL_FRAGMENT_SHADER, shaders_beginning_frag);
    GLuint t2_frag_shader =
        compile_shader(GL_FRAGMENT_SHADER, shaders_two_triangles_second_frag);
    shader_init(&shader[0], vert_shader, t1_frag_shader);
    shader_init(&shader[1], vert_shader, t2_frag_shader);
    delete_shader(vert_shader);
    delete_shader(t1_frag_shader);
    delete_shader(t2_frag_shader);
}

void two_triangles_draw(Program *prog) {
    (void) prog;

    shader_bind(&shader[0]);
    vao_bind(&vao[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    vao_unbind();
    shader_unbind();

    shader_bind(&shader[1]);
    vao_bind(&vao[1]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    vao_unbind();
    shader_unbind();
}

void two_triangles_input(Program *prog) {
    (void) prog;
}

void two_triangles_end(Program *prog) {
    (void) prog;

    for (size_t i = 0; i < 2; ++i) {
        vao_deinit(&vao[i]);
        shader_deinit(&shader[i]);
    }
}
