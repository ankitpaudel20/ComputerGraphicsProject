#pragma once
#include "maths.h"

/**
 * @brief namespace that has all transformation matrices
 */
namespace trans {
static mat3f translate(const vec2 &value) {
    mat3f trans_matrix;
    trans_matrix(0, 2) = value.x;
    trans_matrix(1, 2) = value.y;
    return trans_matrix;
}

static mat4f translate(const vec3 &value) {
    mat4f trans_matrix;
    trans_matrix(0, 3) = value.x;
    trans_matrix(1, 3) = value.y;
    trans_matrix(2, 3) = value.z;
    return trans_matrix;
}

static mat4f x_rotation(float angle) {
    angle *= radian;
    mat4f rot_matrix;
    rot_matrix(1, 1) = std::cos(angle);
    rot_matrix(1, 2) = -std::sin(angle);
    rot_matrix(2, 1) = -rot_matrix(1, 2);
    rot_matrix(2, 2) = rot_matrix(1, 1);
    return rot_matrix;
}

static mat4f y_rotation(float angle) {
    angle *= radian;
    mat4f rot_matrix;
    rot_matrix(2, 2) = std::cos(angle);
    rot_matrix(2, 0) = -std::sin(angle);
    rot_matrix(0, 2) = -rot_matrix(2, 0);
    rot_matrix(0, 0) = rot_matrix(2, 2);
    return rot_matrix;
}

static mat4f z_rotation(float angle) {
    angle *= radian;
    mat4f rot_matrix;
    rot_matrix(0, 0) = std::cos(angle);
    rot_matrix(0, 1) = -std::sin(angle);
    rot_matrix(1, 0) = -rot_matrix(0, 1);
    rot_matrix(1, 1) = rot_matrix(0, 0);
    return rot_matrix;
}

static mat4f rotation(float angle, const vec3 &v) {
    float const a = angle;
    float const c = cos(a);
    float const s = sin(a);

    vec3 axis(vec3::normalize(v));
    vec3 temp(axis * (1 - c));

    mat4f Rotate;
    Rotate(0, 0) = c + temp.x * axis.x;
    Rotate(0, 1) = temp.x * axis.y + s * axis.z;
    Rotate(0, 2) = temp.x * axis.z - s * axis.y;

    Rotate(1, 0) = temp.y * axis.x - s * axis.z;
    Rotate(1, 1) = c + temp.y * axis.y;
    Rotate(1, 2) = temp.y * axis.z + s * axis.x;

    Rotate(2, 0) = temp.z * axis.x + s * axis.y;
    Rotate(2, 1) = temp.z * axis.y - s * axis.x;
    Rotate(2, 2) = c + temp.z * axis.z;
    return Rotate;
}

static mat3f rotation(const float &angle, const vec2 &about = vec2(0)) {
    mat3f rot_matrix;
    rot_matrix(0, 0) = std::cos(angle);
    rot_matrix(0, 1) = -std::sin(angle);
    rot_matrix(1, 0) = -rot_matrix(0, 1);
    rot_matrix(1, 1) = rot_matrix(0, 0);
    if (about == vec2(0))
        return rot_matrix;
    return translate(about) * rot_matrix * translate(-about);
}

static mat3f scaling(const vec2 &value, const vec2 &about = vec2(0), const float &angle_offset = 0) {
    mat3f scale;
    scale(0, 0) = value.x;
    scale(1, 1) = value.y;

    if (angle_offset != 0)
        scale = rotation(angle_offset) * scale * rotation(-angle_offset);
    if (about != vec2(0))
        scale = translate(about) * scale * translate(-about);

    return scale;
}

static mat4f scaling3d(const vec3 &value) {
    mat4f scale;
    scale(0, 0) = value.x;
    scale(1, 1) = value.y;
    scale(2, 2) = value.z;

    return scale;
}

static mat3f shearing(const vec2 &values, const vec2 &ref = vec2(0)) {
    mat3f shear;
    shear(0, 1) = values.x;
    shear(1, 0) = values.y;
    if (ref != vec2(0)) {
        shear(0, 2) = -values.x * ref.y;
        shear(1, 2) = -values.y * ref.x;
    }
    return shear;
}

//input a line in the form of y=mx+c
static mat3f reflection(const float &m, const float &c) {
    float angle = std::atan(m);
    mat3f refl;
    refl(0, 0) = cos(2 * angle);
    refl(0, 1) = sin(2 * angle);
    refl(0, 2) = -c * refl(0, 1);
    refl(1, 0) = refl(0, 1);
    refl(1, 1) = -refl(0, 0);
    refl(1, 2) = c * (1 + refl(0, 0));

    return refl;
}

static mat4f lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
    const vec3 z = (eye - center).normalize();
    const vec3 y = vec3::normalize(up);
    const vec3 x = vec3::cross(y, z).normalize();
    const vec3 c = -center;

    return mat4f({x.x, x.y, x.z, vec3::dot(x, c),
                  y.x, y.y, y.z, vec3::dot(y, c),
                  z.x, z.y, z.z, vec3::dot(z, c),
                  0, 0, 0, 1});
}

static mat4f persp(const float &w, const float &h, const float &fovx, float fovy = 0) {
    float D2R = 3.14159265 / 180.0;

    fovy = fovy == 0 ? fovx * h / w : fovy;
    return mat4f({-w / (2 * tanf(D2R * fovx / 2)), 0, 0, 0,
                  0, -h / (2 * tanf(D2R * fovy / 2)), 0, 0,
                  0, 0, 1, 0,
                  0, 0, 1, 0});
}

static mat4f perspFOV(float fov, float aspect, float n, float f) {
    const float D2R = 3.14159265 / 180.0;
    const auto tan_fov_by2_inv = 1 / tanf(fov * D2R / 2);
    const auto fmn = f - n;
    mat4f ret({tan_fov_by2_inv / aspect, 0, 0, 0,
               0, tan_fov_by2_inv, 0, 0,
               0, 0, -f / fmn, f * n / fmn,
               0, 0, -1, 0});
    return ret;
}

static mat4f perspective(float x, float y, float z, float zvp) {
    float dp = z - zvp;
    if (!dp) {
        printf("DP is zero");
        return mat4f();
    }
    mat4f ret;
    ret(0, 2) = -x / dp;
    ret(0, 3) = x * zvp / dp;
    ret(1, 2) = -1 * y / dp;
    ret(1, 3) = y * zvp / dp;
    ret(2, 2) = 1;
    ret(2, 3) = 0;
    ret(3, 2) = -1 / dp;
    ret(3, 3) = z / dp;

    return ret;
}

// oblique
static mat4f oblique_projection(float alpha, float theta) {
    float pi = 3.14159265;
    if (alpha == 0 || alpha == 180 || alpha == 360)
        return mat4f();
    theta *= pi / 180;
    alpha *= pi / 180;
    float l1 = 1 / tan(alpha);
    mat4f tranMatrix({1, 0, l1 * cosf(theta), 0,
                      0, 1, l1 * sinf(theta), 0,
                      0, 0, 0, 0,
                      0, 0, 0, 1});
    return tranMatrix;
}
} // namespace trans