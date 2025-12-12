#include "texture.h"
#include <assert.h>
#include <stb_image.h>
#include <stdio.h>

void texture_init_from_memory(Texture       *self,
                              uint32_t       unit,
                              const uint8_t *data,
                              size_t         len,
                              GLint          internal_format,
                              GLenum         format) {
    zero(self);

    int32_t  width, height, channels;
    uint8_t *img = stbi_load_from_memory(
        (const uint8_t *) data, (int32_t) len, &width, &height, &channels, 0);

    assert(img && "Failed to load texture.");

    glGenTextures(1, &self->name);
    texture_bind(self, unit);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 internal_format,
                 width,
                 height,
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 img);
    glGenerateMipmap(GL_TEXTURE_2D);
    texture_unbind();

    stbi_image_free(img);
}

void texture_deinit(Texture *self) {
    glDeleteTextures(1, &self->name);

    zero(self);
}

void texture_bind(const Texture *self, uint32_t unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, self->name);
}

void texture_unbind() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
