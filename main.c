
#include <stdio.h>
#include <SDL2/SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>

#define SHADER_LEN 4096

void assert (const char *msg, int cond) {
  if (cond) {
    return;
  }
  fprintf(stderr, "[main] Assertion failed: %s\n", msg);
  exit(1);
}

void readFile (char *target, const char *path) {
  FILE *f = fopen(path, "r");
  assert("Opened shader file successfully", !!f);

  size_t read = fread(target, 1, SHADER_LEN, f);
  assert("Read shader successfully", read > 0);

  fclose(f);
  return;
}

int main(int argc, char *argv[]) {
  printf("[main] Calling SDL_Init\n");
  SDL_Init(SDL_INIT_VIDEO);
  printf("[main] Returned from SDL_Init\n");

  printf("[main] Asking for OpenGL 3.2 context\n");
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_Window* window = SDL_CreateWindow("opengl-inject-poc", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
  assert("Window created correctly", !!window);

  SDL_GLContext context = SDL_GL_CreateContext(window);
  assert("OpenGL context created correctly", !!context);

  printf("[main] Initializing glew...\n");
  glewExperimental = GL_TRUE;
  glewInit();

  float vertices[] = {
    0.0f,  0.5f, // Vertex 1 (X, Y)
    0.5f, -0.5f, // Vertex 2 (X, Y)
    -0.5f, -0.5f  // Vertex 3 (X, Y)
  };

  printf("[main] Making a vertex buffer...\n");
  GLuint vbo;
  glGenBuffers(1, &vbo);
  printf("[main] Vertex buffer: %u\n", vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  char* vertexSource = malloc(SHADER_LEN);
  bzero(vertexSource, SHADER_LEN);
  readFile(vertexSource, "shader.vert");
  printf("vertex source: %s\n", vertexSource);

  char* fragmentSource = malloc(SHADER_LEN);
  bzero(fragmentSource, SHADER_LEN);
  readFile(fragmentSource, "shader.frag");
  printf("fragment source: %s\n", fragmentSource);

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, (void*) &vertexSource, NULL);
  glCompileShader(vertexShader);

  GLint status;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);

  if (status != GL_TRUE) {
    char buffer[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
    fprintf(stderr, "Vertex shader compile log: %s\n", buffer);

    assert("Vertex shader compiled", status == GL_TRUE);
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, (void*) &fragmentSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);

  if (status != GL_TRUE) {
    char buffer[512];
    glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
    fprintf(stderr, "Fragment shader compile log: %s\n", buffer);

    assert("Fragment shader compiled", status == GL_TRUE);
  }

  printf("[main] Sleeping for a second\n");
  SDL_Delay(1000);

  printf("[main] Deleting OpenGL context\n");
  SDL_GL_DeleteContext(context);

  printf("[main] Quitting\n");
  SDL_Quit();

  return 0;
}