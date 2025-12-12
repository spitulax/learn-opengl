#include "prog.h"
#include "shader.h"
#include "utils.h"
#include "vao.h"
#include <math.h>

#define M_PI_2 1.57079632679489661923    // pi/2

include_file(shaders_beginning_vert);
include_file(shaders_shader_triangle_color_edge_frag);
include_file(shaders_shader_triangle_color_edge_vert);
include_file(shaders_shader_triangle_time_frag);

static VAO    vao[2];
static Shader shader[2];

void shader_triangle_start(Program *prog) {
    (void) prog;

    float t1_vertices[] = {
        -0.5f,  -0.5f, 0.0f,    //
        0.0f,   -0.5f, 0.0f,    //
        -0.25f, 0.5f,  0.0f,    //
    };

    /* clang-format off */
    float t2_vertices[] = {
        /* vertices */           /* colors */
        0.0f,  0.5f,  0.0f, /**/ 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.0f, /**/ 0.0f, 1.0f, 0.0f,
        0.25f, -0.5f, 0.0f, /**/ 0.0f, 0.0f, 1.0f,
    };
    /* clang-format on */

    vao_init(&vao[0]);
    vao_add_vbo(&vao[0], t1_vertices, sizeof(t1_vertices), GL_STATIC_DRAW);
    vao_vertex_attrib_ptr(&vao[0], 0, 3, 0, 3 * sizeof(float), GL_FLOAT, false);

    vao_init(&vao[1]);
    vao_add_vbo(&vao[1], t2_vertices, sizeof(t2_vertices), GL_STATIC_DRAW);
    const int32_t stride = 6 * sizeof(float);
    vao_vertex_attrib_ptr(&vao[1], 0, 3, 0, stride, GL_FLOAT, false);
    vao_vertex_attrib_ptr(
        &vao[1], 1, 3, 3 * sizeof(float), stride, GL_FLOAT, false);

    GLuint t1_vert_shader =
        compile_shader(GL_VERTEX_SHADER, shaders_beginning_vert);
    GLuint t1_frag_shader =
        compile_shader(GL_FRAGMENT_SHADER, shaders_shader_triangle_time_frag);
    shader_init(&shader[0], t1_vert_shader, t1_frag_shader);
    delete_shader(t1_vert_shader);
    delete_shader(t1_frag_shader);

    GLuint t2_vert_shader = compile_shader(
        GL_VERTEX_SHADER, shaders_shader_triangle_color_edge_vert);
    GLuint t2_frag_shader = compile_shader(
        GL_FRAGMENT_SHADER, shaders_shader_triangle_color_edge_frag);
    shader_init(&shader[1], t2_vert_shader, t2_frag_shader);
    delete_shader(t2_vert_shader);
    delete_shader(t2_frag_shader);
}

void shader_triangle_draw(Program *prog) {
    (void) prog;

    shader_bind(&shader[0]);
    {
        const double time  = glfwGetTime();
        const double freq  = 5.0;
        const float  green = (float) (sin(time * freq - M_PI_2) + 1.0) * 0.5f;

        GLint loc = shader_get_uniform(&shader[0], "uColor");
        glUniform4f(loc, 0.0f, green, 0.0f, 0.0f);

        vao_bind(&vao[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        vao_unbind();
    }
    shader_unbind();

    shader_bind(&shader[1]);
    {
        vao_bind(&vao[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        vao_unbind();
    }
    shader_unbind();
}

void shader_triangle_input(Program *prog) {
    (void) prog;
}

void shader_triangle_end(Program *prog) {
    (void) prog;

    for (size_t i = 0; i < 2; ++i) {
        vao_deinit(&vao[i]);
        shader_deinit(&shader[i]);
    }
}
