#pragma once

#include "utils.h"

typedef struct {
    GLuint name;
} Shader;

// The input shaders are not deleted by this function.
void  shader_init(Shader *self, GLuint vert_shader, GLuint frag_shader);
void  shader_deinit(Shader *self);
void  shader_bind(const Shader *self);
void  shader_unbind();
GLint shader_get_uniform(const Shader *self, const char *name);

#define compile_shader(type, file_name)                                        \
    compile_shader_manual(type,                                                \
                          #file_name,                                          \
                          ___resources_##file_name,                            \
                          ___resources_##file_name##_len)
GLuint compile_shader_manual(GLenum               type,
                             const char          *file_name,
                             const unsigned char *string,
                             unsigned int         len);
void   delete_shader(GLuint shader);
