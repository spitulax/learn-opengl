#pragma once

#include <glad/glad.h>
#include <stdio.h>
#include <string.h>

#define WINDOW_WIDTH         800
#define WINDOW_HEIGHT        600
#define SHADER_INFO_LOG_SIZE 1024
#define MAX_VAOS             16
#define MAX_SHADERS          16
#define MAX_BUFFERS          16

#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#define zero(ptr)    memset((ptr), 0, sizeof(*(ptr)))
#define panic(...)                                                             \
    {                                                                          \
        eprintf(__VA_ARGS__);                                                  \
        abort();                                                               \
    }
#define scmp(a, b) (strcmp((a), (b)) == 0)


#define return_defer(value)                                                    \
    {                                                                          \
        res = (value);                                                         \
        goto defer;                                                            \
    }

#define include_file(name)                                                     \
    extern unsigned char ___resources_##name[];                                \
    extern unsigned int  ___resources_##name##_len

#define resource(name)     ___resources_##name
#define resource_len(name) ___resources_##name##_len

typedef GLsizeiptr ssize_t;
