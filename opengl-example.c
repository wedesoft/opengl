#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int width = 320;
int height = 240;

const char *vertexSource = "#version 130\n\
in mediump vec3 point;\n\
in mediump vec2 texcoord;\n\
out mediump vec2 UV;\n\
void main()\n\
{\n\
  gl_Position = vec4(point, 1);\n\
  UV = texcoord;\n\
}";

const char *fragmentSource = "#version 130\n\
uniform sampler2D tex;\n\
in mediump vec2 UV;\n\
out mediump vec3 fragColor;\n\
void main()\n\
{\n\
  fragColor = texture(tex, UV).rgb;\n\
}";

GLuint vao;
GLuint vbo;
GLuint idx;

GLfloat vertices[] = {
  -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,
   0.5f, -0.5f,  0.0f, 10.0f,  0.0f,
  -0.5f,  0.5f,  0.0f,  0.0f, 10.0f,
   0.5f,  0.5f,  0.0f, 10.0f, 10.0f
};

unsigned int indices[] = {0, 1, 3, 2};

void handleCompileError(const char *step, GLuint context)
{
  GLint result = GL_FALSE;
  glGetShaderiv(context, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    char buffer[1024];
    glGetShaderInfoLog(context, 1024, NULL, buffer);
    if (buffer[0])
      fprintf(stderr, "%s: %s\n", step, buffer);
  };
}

void handleLinkError(const char *step, GLuint context)
{
  GLint result = GL_FALSE;
  glGetProgramiv(context, GL_LINK_STATUS, &result);
  if (result == GL_FALSE) {
    char buffer[1024];
    glGetProgramInfoLog(context, 1024, NULL, buffer);
    if (buffer[0])
      fprintf(stderr, "%s: %s\n", step, buffer);
  };
}

float chequer[] = {
  0.4f, 0.4f, 0.4f, 1.0f, 1.0f, 1.0f,
  1.0f, 1.0f, 1.0f, 0.4f, 0.4f, 0.4f
};

int main(void)
{
  glfwInit();
  GLFWwindow *window = glfwCreateWindow(width, height, "OpenGL example", NULL, NULL);
  glfwMakeContextCurrent(window);
  glewInit();

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);
  handleCompileError("Vertex shader", vertexShader);

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);
  handleCompileError("Fragment shader", fragmentShader);

  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  handleLinkError("Shader program", program);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
               GL_STATIC_DRAW);
  glGenBuffers(1, &idx);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(glGetAttribLocation(program, "point"),
                        3, GL_FLOAT, GL_FALSE,
                        5 * sizeof(float), (void *)0);
  glVertexAttribPointer(glGetAttribLocation(program, "texcoord"),
                        2, GL_FLOAT, GL_FALSE,
                        5 * sizeof(float), (void *)(3 * sizeof(float)));

  glUseProgram(program);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  GLuint tex;
  glGenTextures(1, &tex);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);
  glUniform1i(glGetUniformLocation(program, "tex"), 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_BGR, GL_FLOAT, chequer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  while (!glfwWindowShouldClose(window)) {
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, (void *)0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  };

  glDisableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &idx);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &vbo);

  glBindVertexArray(0);
  glDeleteVertexArrays(1, &vao);

  glDeleteProgram(program);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glfwTerminate();
  return 0;
}
