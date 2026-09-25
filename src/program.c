#include <glad/glad.h>

// glad must come first
#include <GLFW/glfw3.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

#include "base/definitions.h"

typedef struct Color {
  GLfloat red;
  GLfloat blue;
  GLfloat green;
  GLfloat alpha;
} Color;

#define color_args(color) color.red, color.blue, color.green, color.alpha

const GLint initial_width = 600;
const GLint initial_height = 800;
const Color background_colors[2] = {{1.0f, 0.6f, 0.0f, 1.0f}, {0.4f, 0.2f, 1.0f, 1.0f}};

#define program_abort(...) statement(_program_abort(__FILE__, __LINE__, __func__, __VA_ARGS__))

noreturn void _program_abort(const char *file, int line, const char *func, ...) {
  va_list args;
  va_start(args, func);

  const char *message = va_arg(args, const char *);

  fprintf(stderr, "\n");
  fprintf(stderr, "> ---| Fatal Error |---\n");
  fprintf(stderr, "> Error in %s->%s (line %d)\n", file, func, line);
  fprintf(stderr, "> ");
  vfprintf(stderr, message, args);
  fprintf(stderr, "\n");
  fprintf(stderr, "Terminating program\n");

  va_end(args);

  glfwTerminate();
  exit(EXIT_FAILURE);
}

char *read_shader_file(const char *file_path) {
  char *buffer = calloc(512, sizeof(*buffer));
  FILE *file = fopen(file_path, "r");
  fread(buffer, sizeof(*buffer), 512, file);
  fclose(file);

  return buffer;
}

void error_callback(int error_code, const char *description) {
  fprintf(stderr, "\n");
  fprintf(stderr, "> ---| GLFW Error |---\n");
  fprintf(stderr, "> Error code: %d\n", error_code);
  fprintf(stderr, "> %s\n", description);
  fprintf(stderr, "> --------------------\n");
}

// Called when window resized
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  unused(window);

  printf("Window resized to %dx%d\n", width, height);
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window, int *background_color) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    *background_color = 1 - *background_color;
  }
}

// Check success of compliation/link
void check_shader_compilation(GLuint shader) {
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    GLchar info[512];
    glGetShaderInfoLog(shader, 512, NULL, info);
    fprintf(stderr, "Error compiling shader %d:\n", shader);
    fprintf(stderr, "%s\n", info);

    program_abort("Failed to compile shader");
  }
}

void check_shader_program_linking(GLuint program) {
  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (!success) {
    GLchar info[512];
    glGetProgramInfoLog(program, 512, NULL, info);
    fprintf(stderr, "Error linking shader program %d:\n", program);
    fprintf(stderr, "%s\n", info);

    program_abort("Failed to link shader program");
  }
}

int main(void) {
  /*----------------*/
  /* Set up window */
  /*-------------------------------------------------------------------------------------------------------------*/
  glfwSetErrorCallback(error_callback);
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(initial_width, initial_height, "Test window", NULL, NULL);
  if (window == NULL) {
    program_abort("Failed to create window");
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    program_abort("Failed to load GL functions");
  };

  glViewport(0, 0, initial_width, initial_height);
  /*-------------------------------------------------------------------------------------------------------------*/

  /*----------------*/
  /* Set up shaders */
  /*-------------------------------------------------------------------------------------------------------------*/
  GLchar *vertex_shader_source = read_shader_file("shaders/vertex.glsl");
  GLuint vertex_shader;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, (const GLchar *const *)(&vertex_shader_source), NULL);
  glCompileShader(vertex_shader);
  check_shader_compilation(vertex_shader);
  free(vertex_shader_source);
  vertex_shader_source = NULL;

  GLchar *fragment_shader_source = read_shader_file("shaders/fragment.glsl");
  GLuint fragment_shader;
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, (const GLchar *const *)(&fragment_shader_source), NULL);
  glCompileShader(fragment_shader);
  check_shader_compilation(fragment_shader);
  free(fragment_shader_source);
  fragment_shader_source = NULL;

  GLuint shader_program;
  shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  check_shader_program_linking(shader_program);

  glDeleteShader(vertex_shader);
  vertex_shader = 0;
  glDeleteShader(fragment_shader);
  fragment_shader = 0;
  /*-------------------------------------------------------------------------------------------------------------*/

  /*-----------------*/
  /* Set up vertices */
  /*-------------------------------------------------------------------------------------------------------------*/
  int background_color = 0;
  GLfloat vertices[] = {
      -0.5f, +0.5f, +0.0f,  // Top left
      +0.5f, +0.5f, +1.0f,  // Top right
      -0.5f, -0.5f, +0.0f,  // Bottom left
      +0.5f, -0.5f, +0.0f,  // Bottom right
  };
  GLubyte indices[] = {
      0, 1, 2,  // Top right triangle
      1, 3, 2,  // Bottom right triangle
  };

  GLuint vao_id;
  glGenVertexArrays(1, &vao_id);
  glBindVertexArray(vao_id);

  GLuint vbo_id;
  glGenBuffers(1, &vbo_id);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint ebo_id;
  glGenBuffers(1, &ebo_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Set up vertex attributes to feed into the vertex shader
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(*vertices), (void *)0);
  glEnableVertexAttribArray(0);

#if 0
  // Wireframe mode
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
  /*-------------------------------------------------------------------------------------------------------------*/

  /*-------------*/
  /* Render loop */
  /*-------------------------------------------------------------------------------------------------------------*/
  while (!glfwWindowShouldClose(window)) {
    process_input(window, &background_color);

    // Clear screen
    glClearColor(color_args(background_colors[background_color]));
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the triangle
    glUseProgram(shader_program);
    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  /*-------------------------------------------------------------------------------------------------------------*/

  /*---------*/
  /* Cleanup */
  /*-------------------------------------------------------------------------------------------------------------*/
  glDeleteVertexArrays(1, &vao_id);
  glDeleteBuffers(1, &vbo_id);
  glDeleteBuffers(1, &ebo_id);
  glDeleteProgram(shader_program);

  glfwTerminate();
  /*-------------------------------------------------------------------------------------------------------------*/

  return 0;
}
