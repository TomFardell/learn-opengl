#include <glad/glad.h>

// glad must come first
#include <GLFW/glfw3.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/definitions.h"
#include "shader_program.h"
#include "utils.h"

const char *vertex_shader_file = "shaders/vertex.glsl";
const char *fragment_shader_file = "shaders/fragment.glsl";

constexpr int gl_version_major = 4;
constexpr int gl_version_minor = 5;
constexpr GLint initial_width = 600;
constexpr GLint initial_height = 800;
constexpr Color background_colors[] = {{{1.0f, 0.6f, 0.0f, 1.0f}}, {{0.4f, 0.2f, 1.0f, 1.0f}}};

// Call on GLFW errors
void error_callback(int error_code, const char *description) {
  fprintf(stderr, "\n");
  fprintf(stderr, "> ---| GLFW Error |---\n");
  fprintf(stderr, "> Error code: %d\n", error_code);
  fprintf(stderr, "> %s\n", description);
  fprintf(stderr, "> --------------------\n");
}

// Call when window resized
void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int width, int height) {
  printf("Window resized to %dx%d\n", width, height);
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window, U32 *background_color) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    ++*background_color;
    *background_color %= array_len(background_colors);
  }
}

int main(void) {
  /*---------------------------*/
  /* Set up window and shaders */
  /*-------------------------------------------------------------------------------------------------------------*/
  glfwSetErrorCallback(error_callback);
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_version_major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_version_minor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(initial_width, initial_height, "Test window", NULL, NULL);
  if (window == NULL) {
    program_abort("Failed to create window");
  }
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    program_abort("Failed to initiliase glad");
  };

  glViewport(0, 0, initial_width, initial_height);

  GLuint shader_program = shader_program_init(vertex_shader_file, fragment_shader_file);
  /*-------------------------------------------------------------------------------------------------------------*/

  /*-----------------*/
  /* Set up vertices */
  /*-------------------------------------------------------------------------------------------------------------*/
  U32 background_color = 0;
  Vertex vertices[] = {
      {{-0.5f, +0.5f, +1.0f}, {{+1.0f, +0.1f, +0.1f, +1.0f}}},  // Top left
      {{+0.5f, +0.5f, +1.0f}, {{+0.1f, +1.0f, +0.0f, +1.0f}}},  // Top right
      {{-0.5f, -0.5f, +1.0f}, {{+0.1f, +0.1f, +1.0f, +1.0f}}},  // Bottom left
      {{+0.5f, -0.5f, +1.0f}, {{+1.0f, +1.0f, +0.1f, +1.0f}}},  // Bottom right
  };
  Triangle indices[] = {
      {0, 1, 2},  // Top left triangle
      {1, 3, 2},  // Bottom right triangle
  };

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Set up position vertex attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*vertices), (void *)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);

  // Set up color vertex attribute
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(*vertices), (void *)offsetof(Vertex, color));
  glEnableVertexAttribArray(1);

  // TODO: Pass command line args
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
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 3 * array_len(indices), GL_UNSIGNED_SHORT, NULL);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  /*-------------------------------------------------------------------------------------------------------------*/

  /*---------*/
  /* Cleanup */
  /*-------------------------------------------------------------------------------------------------------------*/
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glDeleteProgram(shader_program);

  glfwTerminate();
  /*-------------------------------------------------------------------------------------------------------------*/

  return 0;
}
