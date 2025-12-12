#include "vao.h"
#include "utils.h"

void vao_init(VAO *self) {
    zero(self);

    glGenVertexArrays(1, &self->name);
}

void vao_bind(const VAO *self) {
    glBindVertexArray(self->name);
}

void vao_unbind() {
    glBindVertexArray(0);
}

void vao_deinit(VAO *self) {
    if (self->name != 0) {
        glDeleteVertexArrays(1, &self->name);
    }

    glDeleteBuffers((GLsizei) self->vbo_len, self->vbos);

    glDeleteBuffers(1, &self->ebo);

    zero(self);
}

void vao_add_vbo(VAO *self, const void *data, GLsizeiptr size, GLenum usage) {
    vao_bind(self);

    GLuint *vbo = self->vbos + self->vbo_len++;
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);

    vao_unbind();
}

void vao_set_ebo(VAO *self, const void *data, ssize_t size, GLenum usage) {
    vao_bind(self);

    glGenBuffers(1, &self->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);

    vao_unbind();
}

void vao_vertex_attrib_ptr(VAO     *self,
                           GLuint   vbo,
                           uint32_t index,
                           int32_t  size,
                           GLenum   type,
                           bool     normalized,
                           int32_t  stride) {
    vao_bind(self);

    glVertexAttribPointer(
        index, size, type, normalized, stride, (void *) (size_t) vbo);
    glEnableVertexAttribArray(index);

    vao_unbind();
}
