#include <GL/glut.h>

int width = 320;
int height = 240;

const char *vertexSource = "#version 130\n\
in mediump vec3 point;\n\
void main()\n\
{\n\
  gl_Position = vec4(point, 1);\n\
}";

const char *fragmentSource = "#version 130\n\
out mediump vec3 fragColor;\n\
void main()\n\
{\n\
  fragColor = vec3(1, 0, 0);\n\
}";

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(width, height);
  glutCreateWindow("OpenGL example");
  glutMainLoop();
  return 0;
}

// gcc -o opengl-example opengl-example.c -lglut
