#include "texture.h"
#include "utils.h"
#include <assert.h>
#include <stb_image.h>
#include <stdint.h>

include_file(shaders_texture_frag);
include_file(shaders_texture_vert);
include_file(textures_container_jpg);

static VAO    *vao;
static Shader *shader;
// TODO: Delete texture.
static GLuint texture;

void texture_setup(Program *prog) {
    /* clang-format off */
    float vertices[] = {
        /* vertices      */ /* texcoords */
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f,         // top right
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,         // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,         // bottom left
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,         // top left
    };
    /* clang-format on */

    uint32_t indices[] = {
        0, 1, 3,    // first triangle
        1, 2, 3,    // second triangle
    };

    vao = program_add_vao(prog);
    vao_add_vbo(vao, vertices, sizeof(vertices), GL_STATIC_DRAW);
    const int32_t stride = 5 * sizeof(float);
    vao_vertex_attrib_ptr(vao, 0, 3, 0, stride, GL_FLOAT, false);
    vao_vertex_attrib_ptr(
        vao, 1, 2, 3 * sizeof(float), stride, GL_FLOAT, false);
    vao_set_ebo(vao, indices, sizeof(indices), GL_STATIC_DRAW);

    int32_t  width, height, channels;
    uint8_t *data =
        stbi_load_from_memory(resource(textures_container_jpg),
                              (int32_t) resource_len(textures_container_jpg),
                              &width,
                              &height,
                              &channels,
                              0);

    assert(data && "Failed to load texture.");

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 width,
                 height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    GLuint vert_shader = compile_shader(GL_VERTEX_SHADER, shaders_texture_vert);
    GLuint frag_shader =
        compile_shader(GL_FRAGMENT_SHADER, shaders_texture_frag);
    shader = program_add_shader(prog, vert_shader, frag_shader);
    delete_shader(vert_shader);
    delete_shader(frag_shader);
}

void texture_draw(const Program *prog) {
    (void) prog;

    shader_bind(shader);
    glBindTexture(GL_TEXTURE_2D, texture);
    vao_bind(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    vao_unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
    shader_unbind();
}
