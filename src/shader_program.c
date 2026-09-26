#include "shader_program.h"

#include <glad/glad.h>
#include <stdio.h>

#include "base/memory.h"
#include "base/string.h"
#include "utils.h"

// Get number of characters in a given file (including a null terminator)
static U64 get_file_length(const char *file_name) {
  FILE *file = fopen(file_name, "r");
  if (!file) {
    program_abort("Unable to open file %s", file_name);
  }

  fseek(file, 0, SEEK_END);
  I64 result = ftell(file);
  fclose(file);

  if (result == -1) {
    program_abort("Error getting file length of %s\n", file_name);
  }

  // Don't care about the null terminator
  return result - 1;
}

// Read the given file into a string
static String get_file_contents(Arena *a, const char *file_name) {
  U64 result_len = get_file_length(file_name);
  char *result_str = arena_alloc_array(a, char, result_len);

  FILE *file = fopen(file_name, "r");
  if (!file) {
    program_abort("Unable to open file %s", file_name);
  }
  fread(result_str, sizeof(*result_str), result_len, file);
  fclose(file);

  return string_init(result_str, result_len);
}

// Check the success of shader compilation
void check_shader_compilation(GLuint shader) {
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    GLchar info[512];
    glGetShaderInfoLog(shader, 512, NULL, info);
    fprintf(stderr, "Error compiling shader %d:\n", shader);
    fprintf(stderr, "%s\n", info);

    program_abort("Failed to compile shader %d", shader);
  }
}

// Check the success of shader program linking
void check_shader_program_linking(GLuint program) {
  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (!success) {
    GLchar info[512];
    glGetProgramInfoLog(program, 512, NULL, info);
    fprintf(stderr, "Error linking shader program %d:\n", program);
    fprintf(stderr, "%s\n", info);

    program_abort("Failed to link shader program %d", program);
  }
}

GLuint shader_program_init(const char *vertex_shader_file, const char *fragment_shader_file) {
  Arena a = arena_init(get_file_length(vertex_shader_file) + get_file_length(fragment_shader_file));

  String vertex_shader_source = get_file_contents(&a, vertex_shader_file);
  String fragment_shader_source = get_file_contents(&a, fragment_shader_file);

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source.str, (GLint *)&vertex_shader_source.len);
  glCompileShader(vertex_shader);
  check_shader_compilation(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source.str, (GLint *)&fragment_shader_source.len);
  glCompileShader(fragment_shader);
  check_shader_compilation(fragment_shader);

  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  check_shader_program_linking(shader_program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  arena_free(&a);

  return shader_program;
}
