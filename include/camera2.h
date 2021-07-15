// camera class
#pragma once
#include "maths.h"

namespace neupane {
float radians(float degree) { return degree * 3.1415 / 180; }

class Camera {
private:
  vec3 position;
  vec3 direction;
  vec3 up;
  float speed;
  int mouseX;
  int mouseY;
  float yaw;
  float pitch;
  bool firstMouse;
  float fov;

public:
  Camera(vec3 position, vec3 direction, vec3 up)
      : position(position), direction(direction), up(up), yaw(-90.f),
        firstMouse(true), fov(45.f), pitch(0), mouseX(1280.0f / 2),
        mouseY(800.0f / 2) {}
  vec3 getPosition() { return position; }
  vec3 getDirection() { return direction; }
  vec3 getUpDirection() { return up; }
  float calcSpeed(float deltatime) {
    speed = 2222.5f * deltatime;
    return speed;
  }
  vec3 moveForeward() {
    // dividing by 10 because i have increase speet factor in above from 22.5 to
    // 222.5 to increase left right top down speed
    position = position + direction * speed / 100;
    return position;
  }
  vec3 moveBack() {
    position = position - direction * speed / 100;
    return position;
  }
  vec3 moveLeft() {
    // speed = 10;
    position = position - vec3::cross(direction, up).normalize() * speed;
    // printf("new position %d", position);
    return position;
  }
  vec3 moveRight() {
    auto rightDir = vec3::cross(direction, up).normalize();
    position = position + vec3::cross(direction, up).normalize() * speed;
    return position;
  }
  vec3 moveUp() {
    position = position + up * speed;
    return position;
  }
  vec3 moveDown() {
    position = position - up * speed;
    return position;
  }
  void setDirection(vec3 dir) {
    printf("set direction called \n");
    direction = dir;
  }

  void onMousePositionChange(int xpos, int ypos) {
    printf("changed\n");
    if (firstMouse) {
      mouseX = xpos;
      mouseY = ypos;
      firstMouse = false;
      return;
    }

    float xoffset = xpos - mouseX;
    float yoffset =
        (mouseY - ypos); // reversed since y-coordinates go from bottom to top
    mouseX = xpos;
    mouseY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;

    vec3 front;
    front.x = cos(radians(yaw)) * cos(radians(pitch));
    front.y = sin(radians(pitch));
    front.z = sin(radians(yaw)) * cos(radians(pitch));
    setDirection(front.normalize());
  }
  void onScroll(double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f)
      fov = 1.0f;
    if (fov > 45.0f)
      fov = 45.0f;
  }
  double getFov() { return fov; }
};
} // namespace neupane
