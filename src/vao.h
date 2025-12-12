#pragma once

#include "utils.h"
#include <glad/glad.h>
#include <stdlib.h>

typedef struct {
    GLuint name;
    GLuint vbos[MAX_BUFFERS];
    size_t vbo_len;
    GLuint ebo;
} VAO;

void vao_init(VAO *self);
void vao_deinit(VAO *self);
void vao_bind(const VAO *self);
void vao_unbind();
void vao_add_vbo(VAO *self, const void *data, ssize_t size, GLenum usage);
void vao_set_ebo(VAO *self, const void *data, ssize_t size, GLenum usage);
void vao_vertex_attrib_ptr(VAO     *self,
                           GLuint   vbo,
                           uint32_t index,
                           int32_t  size,
                           GLenum   type,
                           bool     normalized,
                           int32_t  stride);
