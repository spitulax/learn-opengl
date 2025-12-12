#include "utils.h"

uint32_t compile_shader_manual(GLenum               type,
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
