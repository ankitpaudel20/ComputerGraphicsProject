#pragma once
#include "transformations.h"

struct Euler_angles {
  float m_roll, m_yaw, m_pitch;
};

class camera {
public:
  vec3 viewDir;
  vec3 up;
  float pitchAngle;

  vec3 eye;
  float nearPoint{0.01}, farPoint{1000.0};
  float FOV{90};
  float sensitivity = 0.000005;
  float speed = 0.001;

  camera()
      : viewDir(0.0f, 0.0f, -1.0f), eye(0.0f, 0.0f, 3.0f), up(0.0f, 1.0f, 0.0f),
        pitchAngle(0) {
    checkCorrectness();
  }

  camera(const vec3 &pos, const vec3 &up, const vec3 &dir)
      : viewDir(dir), up(vec3::normalize(up)), eye(pos), pitchAngle(0) {
    checkCorrectness();
  }

  void changeDir(vec3 dir) {
    auto diff = dir.normalize() - viewDir;
    viewDir = std::move(dir);
    up += diff;
    up.normalize();
  }

  void newDelYaw(float angle_in_deg) {
    auto rot = trans::rotation(angle_in_deg * radian, up);
    viewDir = rot * (vec4)viewDir;
    viewDir.normalize();
    up = vec3(0, 1, 0);
    correct();
  }

  void DelRoll(float angle_in_deg) {
    up = trans::rotation(angle_in_deg * radian, (viewDir)) * (vec4)up;
    up.normalize();
  }

  void DelYaw(float angle_in_deg) {
    auto rot = trans::rotation(angle_in_deg * radian, up);
    viewDir = rot * (vec4)viewDir;
    viewDir.normalize();
  }

  void DelPitch(float angle_in_deg) {
    if (pitchAngle + angle_in_deg > 90) {
      pitchAngle = 90;
    } else if (pitchAngle + angle_in_deg < -90) {
      pitchAngle = -90;
    } else {
      vec3 right = vec3::cross(viewDir, up);
      auto rot = trans::rotation(angle_in_deg * radian, right);
      up = vec3(rot * (vec4)up);
      viewDir = vec3(rot * (vec4)viewDir);
      viewDir.normalize();
      auto dot = vec3::dot(viewDir, vec3(0, 1, 0));
      pitchAngle = 90 - acosf(dot) * 180 / pi;
    }
  }

  vec3 getUp() { return up; }
  vec3 getViewDir() { return viewDir; }

private:
  inline void checkCorrectness() {
    auto dot = vec3::dot(viewDir, up);
    if (dot > epsilon && dot < -epsilon) {
      correct();
    }
  }
  inline void correct() {
    up = vec3::normalize(vec3::cross(viewDir, vec3::cross(up, viewDir)));
  }
};
