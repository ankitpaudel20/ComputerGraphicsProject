#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "camera.h"
#include "camera2.h"
#include "engine.h"
#include "helpers.h"
#include "transformations.h"
#include <iostream>
#include <vector>

mat4 translate3d;
mat4 scale3d;
camera cam;
neupane::Camera camera = neupane::Camera(
    vec3(0.0f, 0.0f, 20.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));

static void error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error: %s\n", description);
}

static bool captured = true;
static int mx, my;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  auto dir = cam.center - cam.eye;
  // const float cameraSpeed = 2.5 * s.deltatime;
  vec3 left = vec3::cross(cam.up, dir);
  switch (action) {
  case GLFW_PRESS:
    switch (key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, true);
      break;
    case GLFW_KEY_1:
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      captured = false;
      break;
    case GLFW_KEY_2:
      glfwSetCursorPos(window, mx, my);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      captured = true;
      break;
    case GLFW_KEY_W:
      camera.moveForeward();
    default:
      break;
    }
    break;
  }
}

void processHoldEvent(GLFWwindow *window) {
  auto dir = cam.center - cam.eye;

  // const float cameraSpeed = 2.5 * s.deltatime;
  vec3 left = vec3::cross(cam.up, dir);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.moveForeward();
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    cam.eye -= vec3::normalize(vec3::cross(dir, cam.up)) * cam.speed;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    cam.eye -= dir * cam.speed;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    cam.eye += vec3::normalize(vec3::cross(dir, cam.up)) * cam.speed;
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    cam.eye += cam.up * cam.speed;
  }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    cam.eye -= cam.up * cam.speed;
  }
}

static void cursor_position_callback(GLFWwindow *window, double x, double y) {
  // printf("(x=%d, y=%d)\n", x, y);
  if (captured) {
    float xoffset = mx - x;
    float yoffset = my - y;
    mx = x;
    my = y;

    float sensitivity = 0.09;
    xoffset *= cam.sensitivity;
    yoffset *= cam.sensitivity;

    cam.DelYaw(xoffset);
    cam.DelPitch(yoffset);
  }
}

int window_width, window_height;
engine *graphicsEngine;

static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
  // auto oldWidth = window_width;
  // auto oldHeight = window_height;

  window_height = height;
  window_width = width;

  graphicsEngine->x_size = window_width;
  graphicsEngine->y_size = window_height;
  delete[] graphicsEngine->grid;
  delete[] graphicsEngine->color;
  graphicsEngine->grid = new bool[window_height * window_width];
  graphicsEngine->color = new vec3[window_height * window_width];
  graphicsEngine->clear();

  glViewport(0, 0, window_width, window_height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, window_width, 0.0, window_height);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

int main(int argc, char **argv) {

  GLFWwindow *window;

  if (!glfwInit())
    return -1;
  glfwSetErrorCallback(error_callback);

  window = glfwCreateWindow(640, 480, "main", NULL, NULL);
  if (!(window)) {
    glfwTerminate();
    return -1;
  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwMakeContextCurrent(window);

  GLenum err = glewInit();
  if (GLEW_OK != err) {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
  }

  glfwSwapInterval(1);

  glfwGetFramebufferSize(window, &window_width, &window_height);
  graphicsEngine = new engine(window_width, window_height);

  std::vector<std::vector<int>> edgeMatrix;
  std::vector<std::vector<double>> cube_matix;
  int angle;

  vec3 eye(0, 0, 0);
  vec3 center(0, 0, 0);

  vec3 up(0, 1, 0);
  int yval = 0;

  // const bool moveCamera = false;
  float dummy = 1;

  std::vector<std::vector<float>> vertices;
  std::vector<std::vector<float>> indices;
  loadModel("../cube.obj", vertices, indices);

  /* intialize cube to matrix */
  cube_matix = generateMatrixDouble(4, vertices.size());
  for (int i = 0; i < vertices.size(); i++) {
    for (int j = 0; j < 3; j++) {
      cube_matix[j][i] = vertices[i][j];
    }
  }
  for (int i = 0; i < vertices.size(); i++) {
    cube_matix[3][i] = 1;
  }
  // edgeMatrix = getEdgeMatrix(cube_matix);

  /* create edge matrix from indices loaded from the model */

  for (int i = 0; i < indices.size(); i++) {
    edgeMatrix.push_back(
        std::vector<int>(indices[i].begin(), indices[i].end()));
  }

  angle = 0;

  while (!glfwWindowShouldClose(window)) {
    //       glutKeyboardFunc(keyboardhandler);
    //   /* The passive motion callback for a window is called when
    //     the mouse moves within the window while no mouse buttons are pressed.
    //     */
    //   // glutPassiveMotionFunc(mousehandler);
    //   glutMotionFunc(mousehandler);

    auto view_matrix = lookAt(camera.getPosition(),
                              camera.getPosition() + camera.getDirection(),
                              camera.getUpDirection());

    // //matrix::print2Darray(view_matrix);
    mat4 scaling_matrix = scaling3d(vec3(100, 100, dummy));

    mat4f rotation_matrix_y;
    rotation_matrix_y(0, 0) = cos(angle * 3.1415 / 180);
    rotation_matrix_y(0, 2) = sin(angle * 3.1415 / 180);
    rotation_matrix_y(2, 0) = -1 * sin(angle * 3.1415 / 180);
    rotation_matrix_y(2, 2) = cos(angle * 3.1415 / 180);

    // mat4 rotation_matrix_x = x_rotation(angle);

    // auto prespective_mat = prespective(0, 0, 500, -100);
    mat4 prespective_mat =
        my_PerspectiveFOV(45.0f, 640.0f / 480.0f, 0.1f, 200.0f);

    mat4 a = prespective_mat * view_matrix * scaling_matrix ;
    std::vector<vec4> final;

    for (int i = 0; i < cube_matix[0].size(); i++) {
      auto t = a * vec4(cube_matix[0][i], cube_matix[1][i], cube_matix[2][i],
                        cube_matix[3][i]);
      final.push_back(a * vec4(t));
    }

    vec3_T<float> brescolor(1, 0, 0);

    graphicsEngine->clear();
    // graphicsEngine->drawLines(final_axes);
    graphicsEngine->draw(final, edgeMatrix, camera);
    // graphicsEngine->drawLines(final_cube, brescolor, cube_indices);
    // graphicsEngine.draw_bresenham_adjusted(50, 100, -200, -100, vec3(1, 0,
    // 0));
    graphicsEngine->draw();
    angle++;
    if (angle > 360) {
      angle = 0;
    }
    camera.calcSpeed(0.025);
    // glClear(GL_COLOR_BUFFER_BIT);
    // glBegin(GL_TRIANGLES);
    // glColor4f(1, 0, 0, 0);
    // glVertex2i(0, 0);
    // glColor4f(0, 1, 0, 1);
    // glVertex2i(0, 200);
    // glColor4f(0, 0, 1, 1);
    // glVertex2i(100, 100);
    // glEnd();
    // glFlush();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  delete graphicsEngine;
  return 0;
}