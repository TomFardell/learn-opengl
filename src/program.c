#include <glad/glad.h>

// glad must come first
#include <GLFW/glfw3.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "base/definitions.h"
#include "shader_program.h"
#include "stb/stb_image.h"
#include "utils.h"

const char *vertex_shader_file = "shaders/shader.vert";
const char *fragment_shader_file = "shaders/shader.frag";

const char *texture_file_tiles = "assets/tiles.png";
const char *texture_file_container = "assets/container.jpg";
const char *texture_file_cat = "assets/cat.png";

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

void process_input(GLFWwindow *window, U32 *background_color, F32 *cat_mix) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    ++*background_color;
    *background_color %= array_len(background_colors);
  }

  // TODO: Make these framerate-independent
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    *cat_mix += 0.01;
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    *cat_mix -= 0.01;
  }
}

void set_uniforms(GLuint shader_program, F32 cat_mix) {
  // TODO: Keep track of current program, as querying this every frame is inefficient
  GLint previous_program;
  glGetIntegerv(GL_CURRENT_PROGRAM, &previous_program);
  glUseProgram(shader_program);

  GLfloat rotation = 0.1 * (2 * PI32) * glfwGetTime();
  glUniform1f(glGetUniformLocation(shader_program, "rotation"), rotation);

  glUniform1f(glGetUniformLocation(shader_program, "cat_mix"), cat_mix);

  glUseProgram(previous_program);
}

// Read and load the passed texture file, also generating mipmaps
void load_texture(const char *texture_file) {
  int width, height, channels;
  stbi_set_flip_vertically_on_load(1);
  unsigned char *texture_data = stbi_load(texture_file, &width, &height, &channels, 0);
  if (texture_data == nullptr) {
    program_abort("Unable to load texture file '%s'", texture_file);
  }
  if (channels != 3 && channels != 4) {
    program_abort("Unable to interpret texture file with %d channels", channels);
  }

  printf("Loaded '%s' (%dx%d, %d channels)\n", texture_file, width, height, channels);

  GLenum color_format = (channels == 4) ? GL_RGBA : GL_RGB;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, color_format, width, height, 0, color_format, GL_UNSIGNED_BYTE, texture_data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(texture_data);
}

int main() {
  /*---------------------------*/
  /* Set up window and shaders */
  /*-------------------------------------------------------------------------------------------------------------*/
  glfwSetErrorCallback(error_callback);
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_version_major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_version_minor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(initial_width, initial_height, "Learning OpenGL", nullptr, nullptr);
  if (window == nullptr) {
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
  F32 cat_mix = 0.0f;
  Vertex vertices[] = {
      {{-0.5f, +0.5f, +1.0f}, {-0.5f, +1.5f}, {{+1.0f, +0.0f, +0.0f, +1.0f}}},  // Top left
      {{+0.5f, +0.5f, +1.0f}, {+1.5f, +1.5f}, {{+0.0f, +1.0f, +0.0f, +1.0f}}},  // Top right
      {{-0.5f, -0.5f, +1.0f}, {-0.5f, -0.5f}, {{+0.0f, +0.0f, +1.0f, +1.0f}}},  // Bottom left
      {{+0.5f, -0.5f, +1.0f}, {+1.5f, -0.5f}, {{+1.0f, +0.0f, +1.0f, +1.0f}}},  // Bottom right
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

  // TODO: Make a function to do this and figure out all the sizes
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*vertices), (void *)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(*vertices), (void *)offsetof(Vertex, texture_coords));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(*vertices), (void *)offsetof(Vertex, color));
  glEnableVertexAttribArray(2);

  // TODO: Pass command line args
#if 0
  // Wireframe mode
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
  /*-------------------------------------------------------------------------------------------------------------*/

  /*----------------*/
  /* Set up texures */
  /*-------------------------------------------------------------------------------------------------------------*/
  // TODO: Pull this stuff into a function. In fact, we probably want a texture class
  GLuint texture_tiles;
  glGenTextures(1, &texture_tiles);
  glBindTexture(GL_TEXTURE_2D, texture_tiles);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  load_texture(texture_file_tiles);

  GLuint texture_container;
  glGenTextures(1, &texture_container);
  glBindTexture(GL_TEXTURE_2D, texture_container);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  load_texture(texture_file_container);

  GLuint texture_cat;
  glGenTextures(1, &texture_cat);
  glBindTexture(GL_TEXTURE_2D, texture_cat);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  load_texture(texture_file_cat);

  glUseProgram(shader_program);
  glUniform1i(glGetUniformLocation(shader_program, "texture_tiles"), 0);
  glUniform1i(glGetUniformLocation(shader_program, "texture_container"), 1);
  glUniform1i(glGetUniformLocation(shader_program, "texture_cat"), 2);
  /*-------------------------------------------------------------------------------------------------------------*/

  /*-------------*/
  /* Render loop */
  /*-------------------------------------------------------------------------------------------------------------*/
  while (!glfwWindowShouldClose(window)) {
    process_input(window, &background_color, &cat_mix);

    // Clear screen
    glClearColor(color_args(background_colors[background_color]));
    glClear(GL_COLOR_BUFFER_BIT);

    set_uniforms(shader_program, cat_mix);

    // TODO: Really need to clean up using shader programs
    glUseProgram(shader_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_tiles);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_container);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture_cat);

    glBindVertexArray(vao);

    // Draw the triangle
    glDrawElements(GL_TRIANGLES, 3 * array_len(indices), GL_UNSIGNED_SHORT, nullptr);

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

  glDeleteTextures(1, &texture_tiles);
  glDeleteTextures(1, &texture_container);
  glDeleteTextures(1, &texture_cat);

  glDeleteProgram(shader_program);

  glfwTerminate();
  /*-------------------------------------------------------------------------------------------------------------*/

  return 0;
}
