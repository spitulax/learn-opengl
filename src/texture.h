#pragma once

#include "utils.h"

typedef struct {
    GLuint name;
} Texture;

void texture_init_from_memory(Texture       *self,
                              uint32_t       unit,
                              const uint8_t *data,
                              size_t         len,
                              GLint          internal_format,
                              GLenum         format);
void texture_deinit(Texture *self);
void texture_bind(const Texture *self, uint32_t unit);
void texture_unbind();

#define texture_init_from_resource(                                            \
    self, unit, resource_name, internal_format, format)                        \
    texture_init_from_memory((self),                                           \
                             (unit),                                           \
                             resource(resource_name),                          \
                             resource_len(resource_name),                      \
                             (internal_format),                                \
                             (format))
