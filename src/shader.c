#include "shader.h"
#include "utils.h"

void shader_init(Shader *self, GLuint vert_shader, GLuint frag_shader) {
    zero(self);

    self->name = glCreateProgram();
    glAttachShader(self->name, vert_shader);
    glAttachShader(self->name, frag_shader);
    glLinkProgram(self->name);

    int  success;
    char info_log[SHADER_INFO_LOG_SIZE];
    glGetProgramiv(self->name, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(self->name, SHADER_INFO_LOG_SIZE, NULL, info_log);
        eprintf("Failed to link shader: %s\n", info_log);
    }
}

void shader_deinit(Shader *self) {
    glDeleteProgram(self->name);

    zero(self);
}

void shader_bind(const Shader *self) {
    glUseProgram(self->name);
}

void shader_unbind() {
    glUseProgram(0);
}

GLint shader_get_uniform(const Shader *self, const char *name) {
    return glGetUniformLocation(self->name, name);
}

GLuint compile_shader_manual(GLenum               type,
                             const char          *file_name,
                             const unsigned char *string,
                             unsigned int         len) {
    GLuint shader = glCreateShader(type);
    glShaderSource(
        shader, 1, (const char *const *) &string, (const int32_t *) &len);
    glCompileShader(shader);

    int32_t success;
    char    info_log[SHADER_INFO_LOG_SIZE];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, SHADER_INFO_LOG_SIZE, NULL, info_log);
        eprintf("Failed to compile shader %s: %s\n", file_name, info_log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void delete_shader(GLuint shader) {
    glDeleteShader(shader);
}
