#include <math.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int width = 320;
int height = 240;

const char *vertexSource = "#version 130\n\
uniform mat3 rotz;\n\
uniform mat3 rotx;\n\
uniform mat4 projection;\n\
uniform float distance;\n\
in mediump vec3 point;\n\
in mediump vec2 texcoord;\n\
out mediump vec2 UV;\n\
void main()\n\
{\n\
  vec3 pos = rotx * rotz * point;\n\
  pos.z -= distance;\n\
  gl_Position = projection * vec4(pos, 1);\n\
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
  -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
   0.5f, -0.5f,  0.0f, 6.0f, 0.0f,
  -0.5f,  0.5f,  0.0f, 0.0f, 6.0f,
   0.5f,  0.5f,  0.0f, 6.0f, 6.0f
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

  glDepthFunc(GL_GEQUAL);
  glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
  glEnable(GL_DEPTH_TEST);

  float alpha = 30 * M_PI / 180;
  float ca = cos(alpha);
  float sa = sin(alpha);
  float rotz[9] = {ca, sa, 0, -sa, ca, 0, 0, 0, 1};
  glUniformMatrix3fv(glGetUniformLocation(program, "rotz"), 1, GL_TRUE, rotz);

  float beta = 60 * M_PI / 180;
  float cb = cos(beta);
  float sb = sin(beta);
  float rotx[9] = {1, 0, 0, 0, cb, sb, 0, -sb, cb};
  glUniformMatrix3fv(glGetUniformLocation(program, "rotx"), 1, GL_TRUE, rotx);

  glUniform1f(glGetUniformLocation(program, "distance"), 1.8);

  float fov = 45.0 * M_PI / 180;
  float near = 0.1;
  float far = 10.0;
  float dx = 1.0 / tan(0.5 * fov);
  float dy = dx * width / height;
  float a = far * near / (far - near);
  float b = near / (far - near);
  float projection[16] = {dx, 0, 0, 0, 0, dy, 0, 0, 0, 0, b, a, 0, 0, -1, 0};
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, projection);

  while (!glfwWindowShouldClose(window)) {
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    glClearDepth(0.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, (void *)0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  };

  glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteTextures(1, &tex);

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
