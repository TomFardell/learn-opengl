#ifndef UTILS_H
#define UTILS_H

#include <stdnoreturn.h>

#include "glad/glad.h"

typedef union Color {
  struct {
    GLfloat r;
    GLfloat b;
    GLfloat g;
    GLfloat a;
  };

  GLfloat data[4];
} Color;

typedef struct Vertex {
  GLfloat position[3];
  GLfloat texture_coords[2];
  Color color;
} Vertex;

typedef GLushort Triangle[3];

#define color_args(color) color.r, color.b, color.g, color.a

#define program_abort(...) statement(_program_abort(__FILE__, __LINE__, __func__, __VA_ARGS__))
noreturn void _program_abort(const char *file, int line, const char *func, ...);

#endif  // UTILS_H

// vim: filetype=c :
