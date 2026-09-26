#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <glad/glad.h>

// Initialise a shader program by compiling and linking the passed shader files
GLuint shader_program_init(const char *vertex_shader_file, const char *fragment_shader_file);

#endif  // SHADER_PROGRAM_H

// vim: filetype=c :
