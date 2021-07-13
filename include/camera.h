#pragma once
#include "transformations.h"

struct Euler_angles {
    float m_roll, m_yaw, m_pitch;
};

struct camera {
    vec3 center;
    vec3 eye;
    vec3 up;
    float nearPoint{0.01}, farPoint{1000.0};
    float FOV{90};
    float sensitivity = 0.1;
    float speed = 0.05;

    camera() : center(0.0f, 0.0f, -1.0f), eye(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f) {
        checkCorrectness();
    }

    camera(const vec3 &pos, const vec3 &up, const vec3 &dir)
        : center(pos + dir), up(vec3::normalize(up)), eye(pos) {
        checkCorrectness();
    }

    void ChangeDir(const vec3 &dir) {
        center = eye + dir;
    }

    void DelRoll(float angle_in_deg) {
        up = rotation3D(angle_in_deg * radian, (center - eye)) * vec4(up);
    }

    void DelYaw(float angle_in_deg) {
        auto dir = center - eye;
        dir = rotation3D(angle_in_deg * radian, up) * vec4(dir);
        center = eye + dir;
    }

    void DelPitch(float angle_in_deg) {
        auto dir = center - eye;
        auto rot = rotation3D(angle_in_deg * radian, -vec3::cross(dir, up));
        up = rot * vec4(up);
        dir = rot * vec4(dir);
        center = eye + dir;
    }

  private:
    inline void checkCorrectness() {
        auto dir = center - eye;
        auto dot = dir * up;
        if (dot > epsilon && dot < -epsilon) {
            auto perp = vec3::cross(dir, up);
            up = vec3::cross(dir, perp);
        }
    }
};
