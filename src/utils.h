#ifndef UTILS_H
#define UTILS_H

#include <stdnoreturn.h>

#include "glad/glad.h"

typedef struct Color {
  GLfloat red;
  GLfloat blue;
  GLfloat green;
  GLfloat alpha;
} Color;

#define color_args(color) color.red, color.blue, color.green, color.alpha

#define program_abort(...) statement(_program_abort(__FILE__, __LINE__, __func__, __VA_ARGS__))
noreturn void _program_abort(const char *file, int line, const char *func, ...);

#endif  // UTILS_H

// vim: filetype=c :
