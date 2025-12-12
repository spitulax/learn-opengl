#include "prog.h"
#include "shader.h"
#include "utils.h"
#include "vao.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

include_file(shaders_beginning_frag);
include_file(shaders_circle_vert);

#define M_PI 3.14159265358979323846

static VAO    vao;
static Shader shader;

#define DETAILS 32
#define RADIUS  0.1f
#define SPEED   0.01f

static float x     = 0.0f;
static float y     = 0.0f;
static float dir_x = 0.0f;
static float dir_y = 0.0f;

// Reserve first 3 for the central vertex at (0, 0, 0)
static int32_t vertices_len = 3;

static void normalize(float *x, float *y) {
    const float len = (float) sqrt(*x * *x + *y * *y);
    *x /= len;
    *y /= len;
}

void circle_start(Program *prog) {
    (void) prog;

    float   vertices[1024] = { 0 };
    int32_t indices[1024]  = { 0 };

    int32_t indices_len      = 0;
    int32_t vertex_total_len = 1;
    for (size_t i = 0; i < DETAILS; ++i) {
        const double angle = 2 * M_PI / DETAILS * (double) i;

        vertices[vertices_len++] = (float) (sin(angle) * RADIUS);
        vertices[vertices_len++] = (float) (cos(angle) * RADIUS);
        vertices[vertices_len++] = 0.0f;
        if (i > 0) {
            indices[indices_len++] = vertex_total_len;
        }
        ++vertex_total_len;

        indices[indices_len++] = 0;
        indices[indices_len++] = vertex_total_len - 1;
    }
    indices[indices_len++] = 1;

    vao_init(&vao);
    vao_add_vbo(&vao,
                vertices,
                (ssize_t) (vertices_len * (int32_t) sizeof(float)),
                GL_STATIC_DRAW);
    vao_vertex_attrib_ptr(&vao, 0, 3, 0, 3 * sizeof(float), GL_FLOAT, false);
    vao_set_ebo(&vao,
                indices,
                (ssize_t) (indices_len * (int32_t) sizeof(int32_t)),
                GL_STATIC_DRAW);

    GLuint vert_shader = compile_shader(GL_VERTEX_SHADER, shaders_circle_vert);
    GLuint frag_shader =
        compile_shader(GL_FRAGMENT_SHADER, shaders_beginning_frag);
    shader_init(&shader, vert_shader, frag_shader);
    delete_shader(vert_shader);
    delete_shader(frag_shader);

    srand((unsigned int) time(NULL));
    dir_x = (float) rand() / (float) RAND_MAX * 2.0f - 1.0f;
    dir_y = (float) rand() / (float) RAND_MAX * 2.0f - 1.0f;
    normalize(&dir_x, &dir_y);
}

void circle_draw(Program *prog) {
    (void) prog;

    shader_bind(&shader);
    {
        if (x + RADIUS >= 1.0f || x - RADIUS <= -1.0f) {
            dir_x = -dir_x;
        }
        if (y + RADIUS >= 1.0f || y - RADIUS <= -1.0f) {
            dir_y = -dir_y;
        }

        x += dir_x * SPEED;
        y += dir_y * SPEED;

        GLint loc = shader_get_uniform(&shader, "uOffset");
        glUniform2f(loc, x, y);

        vao_bind(&vao);
        glDrawElements(GL_TRIANGLES, vertices_len, GL_UNSIGNED_INT, 0);
        vao_unbind();
    }
    shader_unbind();
}

void circle_input(Program *prog) {
    (void) prog;
}

void circle_end(Program *prog) {
    (void) prog;

    vao_deinit(&vao);
    shader_deinit(&shader);
}
