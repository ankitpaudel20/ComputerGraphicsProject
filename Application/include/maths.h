#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
#include <cstring>
#include <array>
#include "glm/glm.hpp"

#ifdef _MSC_VER
#define DEBUG_BREAK __debugbreak()

#ifdef _DEBUG
#define MDEBUG
#endif
#else
#define DEBUG_BREAK __builtin_trap()

#ifndef NDEBUG
#define MDEBUG
#endif
#endif

constexpr float radian = 3.14159265f / 180.f;
constexpr float epsilon = 1e-4f;
constexpr float pi = 3.14159265f;

template <class T>
struct vec4_T;
template <class T>
struct vec3_T;

template <class T>
struct vec2_T {
    T x, y;

    vec2_T() { x = y = 0; }
    vec2_T(T a) : x(a), y(a) {}
    vec2_T(T a, T b) : x(a), y(b) {}
    vec2_T(const glm::vec2 &in) : x(in.x), y(in.y) {}
    vec2_T(glm::vec2 &in) : x(in.x), y(in.y) {}
    vec2_T(const vec3_T<T> &in) : x(in.x), y(in.y) {}

    template <class U>
    vec2_T(const vec2_T<U> &in) : x(in.x), y(in.y) {}

    float getmag() const {
        return sqrtf(pow(x, 2) + pow(y, 2));
    }

    operator glm::vec4() {
        return glm::vec4(x, y, 1, 1.0);
    }

    operator glm::vec2() {
        return glm::vec2(x, y, 1.0);
    }

    vec2_T<T> operator+(const vec2_T<T> &b) const {
        return vec2_T<T>(x + b.x, y + b.y);
    }

    vec2_T<T> operator+(T a) const {
        return vec2_T<T>(x + a, y + a);
    }

    vec2_T<T> operator-(const vec2_T<T> &b) const {
        return vec2_T<T>(x - b.x, y - b.y);
    }

    vec2_T<T> operator-(T a) const {
        return vec2_T<T>(x - a, y - a);
    }

    vec2_T<T> operator*(const vec2_T<T> &b) const {
        return vec2_T<T>(x * b.x, y * b.y);
    }

    vec2_T<float> operator*(float a) const {
        return vec2_T<float>(x * a, y * a);
    }

    vec2_T<T> operator/(const vec2_T<T> &b) const {
        return vec2_T<T>(x / b.x, y / b.y);
    }

    vec2_T<T> operator/(T a) const {
        return vec2_T<T>(x / a, y / a);
    }

    vec2_T<T> operator-() const {
        return vec2_T<T>(-x, -y);
    }

    vec2_T<T> operator+=(const vec2_T<T> &in) {
        x += in.x;
        y += in.y;
        return *this;
    }

    vec2_T<T> operator-=(const vec2_T<T> &in) {
        x -= in.x;
        y -= in.y;
        return *this;
    }

    vec2_T<T> operator*=(const vec2_T<T> &in) {
        x *= in.x;
        y *= in.y;
        return *this;
    }

    vec2_T<T> operator/=(const vec2_T<T> &in) {
        x /= in.x;
        y /= in.y;
        return *this;
    }

    inline bool operator==(const vec2_T<T> &in) const {
        return x == in.x && y == in.y;
    }

    inline bool operator!=(const vec2_T<T> &in) const {
        return !operator==(in);
    }

    friend std::ostream &operator<<(std::ostream &out, const vec2_T<T> &vec) {
        out << "( " << vec.x << ", " << vec.y << " )";
        return out;
    }

    vec2_T<T> normalize() {
        T temp = sqrt(x * x + y * y);
        *this = *this / temp;
        return *this;
    }

    static vec2_T<T> normalize(const vec2_T<T> &in) {
        T temp = sqrt(in.x * in.x + in.y * in.y);
        return vec2_T<T>(in / temp);
    }

    static double dist(const vec2_T<T> &a, const vec2_T<T> &b) {
        return sqrtf(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
    }

    static T dot(const vec2_T<T> &a, const vec2_T<T> &b) {
        return a.x * b.x + a.y * b.y;
    }
};
typedef vec2_T<float> vec2;

template <class T>
struct vec3_T {
    T x, y, z;

    vec3_T() { x = y = z = 0; }
    vec3_T(T a) : x(a), y(a), z(a) {}
    vec3_T(T a, T b, T c) : x(a), y(b), z(c) {}
    vec3_T(const glm::vec3 &in) : x(in.x), y(in.y), z(in.z) {}
    vec3_T(const vec2_T<T> &in) : x(in.x), y(in.y), z(1) {}
    vec3_T(const vec4_T<T> &in) : x(in.x), y(in.y), z(in.z) {}

    template <class U>
    vec3_T(vec3_T<U> &in) : x(in.x), y(in.y), z(in.z) {}

    float getmag() const {
        return sqrtf(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }

    operator glm::vec4() {
        return glm::vec4(x, y, z, 1.0);
    }

    operator glm::vec3() {
        return glm::vec3(x, y, z);
    }

    vec3_T<T> operator+(const vec3_T<T> &b) const {
        return vec3_T<T>(x + b.x, y + b.y, z + b.z);
    }

    vec3_T<T> operator+(T a) const {
        return vec3_T<T>(x + a, y + a, z + a);
    }

    vec3_T<T> operator-(const vec3_T<T> &b) const {
        return vec3_T<T>(x - b.x, y - b.y, z - b.z);
    }

    vec3_T<T> operator-(T a) const {
        return vec3_T<T>(x - a, y - a, z - a);
    }

    vec3_T<T> operator*(const vec3_T<T> &b) const {
        return vec3_T<T>(x * b.x, y * b.y, z * b.z);
    }

    vec3_T<T> operator*(const T &a) const {
        return vec3_T<T>(x * a, y * a, z * a);
    }

    vec3_T<T> operator/(const vec3_T<T> &b) const {
        return vec3_T<T>(x / b.x, y / b.y, z / b.z);
    }

    vec3_T<T> operator/(T a) const {
        return vec3_T<T>(x / a, y / a, z / a);
    }

    vec3_T<T> operator-() const {
        return vec3_T<T>(-x, -y, -z);
    }

    vec3_T<T> operator+=(const vec3_T<T> &in) {
        x += in.x;
        y += in.y;
        z += in.z;
        return *this;
    }

    vec3_T<T> operator-=(const vec3_T<T> &in) {
        x -= in.x;
        y -= in.y;
        z -= in.z;
        return *this;
    }

    vec3_T<T> operator*=(const vec3_T<T> &in) {
        x *= in.x;
        y *= in.y;
        z *= in.z;
        return *this;
    }

    vec3_T<T> operator/=(const vec3_T<T> &in) {
        x /= in.x;
        y /= in.y;
        z /= in.z;
        return *this;
    }

    inline bool operator==(const vec3_T<T> &in) const {
        return x == in.x && y == in.y && z == in.z;
    }

    inline bool operator!=(const vec3_T<T> &in) const {
        return !operator==(in);
    }

    friend std::ostream &operator<<(std::ostream &out, const vec3_T<T> &vec) {
        out << "( " << vec.x << ", " << vec.y << ", " << vec.z << " )";
        return out;
    }

    vec3_T<T> normalize() {
        T temp = sqrt(x * x + y * y + z * z);
        *this = *this / temp;
        return *this;
    }

    static vec3_T<T> normalize(const vec3_T<T> &in) {
        T temp = sqrt(in.x * in.x + in.y * in.y + in.z * in.z);
        return vec3_T<T>(in / temp);
    }

    static double dist(const vec3_T<T> &a, const vec3_T<T> &b) {
        return sqrtf(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2));
    }

    static vec3_T<T> cross(const vec3_T<T> &a, const vec3_T<T> &b) {
        vec3_T<T> ret;
        ret.x = a.y * b.z - b.y * a.z;
        ret.y = b.x * a.z - a.x * b.z;
        ret.z = a.x * b.y - b.x * a.y;
        return ret.normalize();
    }

    static T dot(const vec3_T<T> &a, const vec3_T<T> &b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
};
typedef vec3_T<float> vec3;

template <class T>
struct vec4_T {
    T x, y, z, w;

    vec4_T() { x = y = z = w = 0; }
    vec4_T(T a) : x(a), y(a), z(a), w(a) {}
    vec4_T(T a, T b, T c, T d) : x(a), y(b), z(c), w(d) {}
    vec4_T(const glm::vec4 &in) : x(in.x), y(in.y), z(in.z), w(in.w) {}
    vec4_T(glm::vec4 &in) : x(in.x), y(in.y), z(in.z), w(in.w) {}
    vec4_T(const vec3_T<T> &in, const T &iw = 1) : x(in.x), y(in.y), z(in.z), w(iw) {}

    float getmag() const {
        return sqrtf(pow(x, 2) + pow(y, 2) + pow(z, 2) + pow(w, 2));
    }

    operator glm::vec4() {
        return glm::vec4(x, y, z, w);
    }
    vec4_T<T> operator+(const vec4_T<T> &b) const {
        return vec4_T<T>(x + b.x, y + b.y, z + b.z, w + b.w);
    }

    vec4_T<T> operator+(const T &a) const {
        return vec4_T<T>(x + a, y + a, z + a, w + a);
    }

    vec4_T<T> operator-(const vec4_T<T> &b) const {
        return vec4_T<T>(x - b.x, y - b.y, z - b.z, w - b.w);
    }

    vec4_T<T> operator-(T a) const {
        return vec4_T<T>(x - a, y - a, z - a, w - a);
    }

    vec4_T<T> operator*(const T &a) const {
        return vec4_T<T>(x * a, y * a, z * a, w * a);
    }
    vec4_T<T> operator/(const vec4_T<T> &b) const {
        return vec4_T<T>(x / b.x, y / b.y, z / b.z, w / b.w);
    }

    vec4_T<T> operator/(T a) const {
        return vec4_T<T>(x / a, y / a, z / a, w / a);
    }

    vec4_T<T> operator-() const {
        return vec4_T<T>(-x, -y, -z, -w);
    }

    vec4_T<T> operator+=(const vec4_T<T> &in) {
        x += in.x;
        y += in.y;
        z += in.z;
        w += in.w;
        return *this;
    }

    vec4_T<T> operator-=(const vec4_T<T> &in) {
        x -= in.x;
        y -= in.y;
        z -= in.z;
        w -= in.w;
        return *this;
    }

    vec4_T<T> operator*=(const vec4_T<T> &in) {
        x *= in.x;
        y *= in.y;
        z *= in.z;
        w *= in.w;
        return *this;
    }

    vec4_T<T> operator/=(const vec4_T<T> &in) {
        x /= in.x;
        y /= in.y;
        z /= in.z;
        w /= in.w;
        return *this;
    }

    inline bool operator==(const vec4_T<T> &in) const {
        return x == in.x && y == in.y && z == in.z && w == in.w;
    }

    inline bool operator!=(const vec4_T<T> &in) const {
        return !operator==(in);
    }

    friend std::ostream &operator<<(std::ostream &out, const vec4_T<T> &vec) {
        out << "( " << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << " )";
        return out;
    }

    vec4_T<T> normalize() {
        T temp = sqrt(x * x + y * y + z * z + w * w);
        *this = *this / temp;
        return *this;
    }

    static vec4_T<T> normalize(const vec4_T<T> &in) {
        T temp = sqrt(in.x * in.x + in.y * in.y + in.z * in.z + in.w * in.w);
        return vec4_T<T>(in / temp);
    }

    static double dist(const vec4_T<T> &a, const vec4_T<T> &b) {
        return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2) + pow(b.w - a.w, 2));
    }

    static T dot(const vec4_T<T> &a, const vec4_T<T> &b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }
};
typedef vec4_T<float> vec4;

template <class Type, size_t x_dim, size_t y_dim>
class mat {
  protected:
    std::array<Type, x_dim * y_dim> m_data;

  public:
    mat() {
        for (int i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                at(i, j) = i == j ? 1 : 0;
    }

    mat(const Type &in) {
        for (int i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                at(i, j) = in;
    }

    mat(const std::array<Type, x_dim * y_dim> &in) {
        assert(in.size() == x_dim * y_dim);
        m_data = in;
    }

    template <class U>
    mat(const mat<U, x_dim, y_dim> &in) {
        for (int i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                at(i, j) = in(i, j);
    }

    inline Type &operator()(const size_t &r, const size_t &c) {
        assert(r >= 0 && r < y_dim && c >= 0 && c < x_dim);
        return m_data[r * x_dim + c];
    }

    inline const Type &operator()(const size_t &r, const size_t &c) const {
        assert(r >= 0 && r < y_dim && c >= 0 && c < x_dim);
        return m_data[r * x_dim + c];
    }

    inline const Type &at(const size_t &r, const size_t &c) const {
        return m_data[r * x_dim + c];
    }

    inline Type &at(const size_t &r, const size_t &c) {
        return m_data[r * x_dim + c];
    }

    void print() const {
        for (size_t i = 0; i < y_dim; i++) {
            printf("(");
            for (size_t j = 0; j < x_dim; j++)
                printf("%8.3lf", at(i, j));
            printf(")\n");
        }
        // printf("\n");
    }

    mat<Type, x_dim, y_dim> toUpper() {
        mat<Type, x_dim, y_dim> temp = *this;
        size_t i, j, k;
        Type constant;
        for (i = 0; i < y_dim; i++) {
            constant = temp(i, i);
            for (j = i + 1; j < y_dim; j++) {
                constant = temp(j, i) / temp(i, i);
                for (k = i; k < x_dim; k++)
                    temp(j, k) = temp(j, k) - constant * temp(i, k);
            }
        }
        return temp;
    }

    mat<Type, x_dim, y_dim> inverse() const {
        assert(x_dim == y_dim);
        mat<Type, x_dim, y_dim> temp = *this;
        mat<Type, x_dim, y_dim> inv;

        auto determinant = this->determinant();
        if (determinant < 1e-7 && determinant > -1e-7) {
            printf("determinant is zero \n");
            throw std::runtime_error("inverse of the doesnot exist");
        }

        size_t i, j, k;

        Type constant;

        for (i = 0; i < y_dim; ++i) {
            if (temp(i, i) == 0) {
                for (j = i; j < y_dim; j++) {
                    if (temp.at(j, j) != 0) {
                        auto dest = &temp.at(j, 0);
                        auto src = &temp.at(i, 0);

                        auto *swaptemp = new Type[x_dim];

                        memcpy(swaptemp, dest, x_dim * sizeof(Type));
                        memcpy(dest, src, x_dim * sizeof(Type));
                        memcpy(src, swaptemp, x_dim * sizeof(Type));

                        dest = &inv(j, 0);
                        src = &inv(i, 0);

                        memcpy(swaptemp, dest, x_dim * sizeof(Type));
                        memcpy(dest, src, x_dim * sizeof(Type));
                        memcpy(src, swaptemp, x_dim * sizeof(Type));

                        delete[] swaptemp;
                    }
                }
            }

            constant = temp(i, i);
            for (j = i; j < x_dim; j++)
                temp(i, j) /= constant;
            for (j = 0; j < x_dim; j++)
                inv(i, j) /= constant;

            for (j = 0; j < y_dim; j++) {
                if (j == i)
                    continue;
                constant = temp(j, i);
                for (k = i; k < x_dim; k++) {
                    temp(j, k) = temp(j, k) - temp(i, k) * constant;
                }

                for (k = 0; k < x_dim; k++) {
                    inv(j, k) = inv(j, k) - inv(i, k) * constant;
                }
            }
        }
        return inv;
    }

    mat<Type, x_dim, y_dim> operator-() {
        mat<Type, x_dim, y_dim> ret = *this;
        for (size_t i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                ret(i, j) = -ret(i, j);
        return ret;
    }

    mat<Type, x_dim, y_dim> operator+(const mat<Type, x_dim, y_dim> &in) {
        mat<Type, x_dim, y_dim> ret;
        for (size_t i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                ret(i, j) = in(i, j) + at(i, j);
        return ret;
    }

    mat<Type, x_dim, y_dim> operator-(const mat<Type, x_dim, y_dim> &in) {
        mat<Type, x_dim, y_dim> ret;
        for (size_t i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                ret(i, j) = in(i, j) - at(i, j);
        return ret;
    }

    void operator+=(const mat<Type, x_dim, y_dim> &in) {
        for (size_t i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                at(i, j) += in(i, j);
    }

    void operator-=(const mat<Type, x_dim, y_dim> &in) {
        for (size_t i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                at(i, j) -= in(i, j);
    }

    void operator*=(const mat<Type, x_dim, y_dim> &in) {
        assert(x_dim == y_dim);
        std::array<Type, x_dim * y_dim> new_buffer;
        int i, j, k;
        for (i = 0; i < y_dim; i++) {
            for (j = 0; j < y_dim; j++) {
                new_buffer[i * y_dim + j] = 0;
                for (k = 0; k < y_dim; k++)
                    new_buffer[i * y_dim + j] += at(i, k) * in(k, j);
            }
        }
        m_data = std::move(new_buffer);
    }

    void operator*=(const Type &a) {
        for (size_t i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                at(i, j) *= a;
    }

    void operator/=(const Type &a) {
        for (size_t i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                at(i, j) /= a;
    }

    vec2_T<Type> operator*(const vec2_T<Type> &in) const {
        assert(x_dim == 2 && y_dim == 2);
        vec2_T<Type> ret;
        ret.x = at(0, 0) * in.x + at(0, 1) * in.y;
        ret.y = at(1, 0) * in.x + at(1, 1) * in.y;
        return ret;
    }

    mat<Type, x_dim, y_dim> operator*(const Type &a) const {
        mat<Type, x_dim, y_dim> ret = *this;
        for (auto &i : ret.m_data)
            i *= a;
        return ret;
    }

    mat<Type, x_dim, y_dim> operator/(const Type &a) const {
        mat<Type, x_dim, y_dim> ret = *this;
        for (auto &i : ret.m_data)
            i /= a;
        return ret;
    }

    Type determinant() {
        assert(x_dim == y_dim);
        mat<Type, x_dim, y_dim> temp = toUpper();
        Type ret = 1;
        for (size_t i = 0; i < y_dim; i++)
            ret *= temp(i, i);

        return ret;
    }
};

template <class Type, size_t x1, size_t y1, size_t x2>
mat<Type, x2, y1> operator*(const mat<Type, x1, y1> &first, const mat<Type, x2, x1> &second) {
    mat<Type, x2, y1> ret;
    int i, j, k;
    for (i = 0; i < y1; i++) {
        for (j = 0; j < x2; j++) {
            ret.at(i, j) = 0;
            for (k = 0; k < x1; k++)
                ret.at(i, j) += first(i, k) * second(k, j);
        }
    }
    return ret;
}

template <class Type>
mat<Type, 4, 4> operator*(const mat<Type, 4, 4> &first, const mat<Type, 4, 4> &second) {
    return mat<Type, 4, 4>({first.at(0, 0) * second.at(0, 0) + first.at(0, 1) * second.at(1, 0) + first.at(0, 2) * second.at(2, 0) + first.at(0, 3) * second.at(3, 0),
                            first.at(0, 0) * second.at(0, 1) + first.at(0, 1) * second.at(1, 1) + first.at(0, 2) * second.at(2, 1) + first.at(0, 3) * second.at(3, 1),
                            first.at(0, 0) * second.at(0, 2) + first.at(0, 1) * second.at(1, 2) + first.at(0, 2) * second.at(2, 2) + first.at(0, 3) * second.at(3, 2),
                            first.at(0, 0) * second.at(0, 3) + first.at(0, 1) * second.at(1, 3) + first.at(0, 2) * second.at(2, 3) + first.at(0, 3) * second.at(3, 3),
                            first.at(1, 0) * second.at(0, 0) + first.at(1, 1) * second.at(1, 0) + first.at(1, 2) * second.at(2, 0) + first.at(1, 3) * second.at(3, 0),
                            first.at(1, 0) * second.at(0, 1) + first.at(1, 1) * second.at(1, 1) + first.at(1, 2) * second.at(2, 1) + first.at(1, 3) * second.at(3, 1),
                            first.at(1, 0) * second.at(0, 2) + first.at(1, 1) * second.at(1, 2) + first.at(1, 2) * second.at(2, 2) + first.at(1, 3) * second.at(3, 2),
                            first.at(1, 0) * second.at(0, 3) + first.at(1, 1) * second.at(1, 3) + first.at(1, 2) * second.at(2, 3) + first.at(1, 3) * second.at(3, 3),
                            first.at(2, 0) * second.at(0, 0) + first.at(2, 1) * second.at(1, 0) + first.at(2, 2) * second.at(2, 0) + first.at(2, 3) * second.at(3, 0),
                            first.at(2, 0) * second.at(0, 1) + first.at(2, 1) * second.at(1, 1) + first.at(2, 2) * second.at(2, 1) + first.at(2, 3) * second.at(3, 1),
                            first.at(2, 0) * second.at(0, 2) + first.at(2, 1) * second.at(1, 2) + first.at(2, 2) * second.at(2, 2) + first.at(2, 3) * second.at(3, 2),
                            first.at(2, 0) * second.at(0, 3) + first.at(2, 1) * second.at(1, 3) + first.at(2, 2) * second.at(2, 3) + first.at(2, 3) * second.at(3, 3),
                            first.at(3, 0) * second.at(0, 0) + first.at(3, 1) * second.at(1, 0) + first.at(3, 2) * second.at(2, 0) + first.at(3, 3) * second.at(3, 0),
                            first.at(3, 0) * second.at(0, 1) + first.at(3, 1) * second.at(1, 1) + first.at(3, 2) * second.at(2, 1) + first.at(3, 3) * second.at(3, 1),
                            first.at(3, 0) * second.at(0, 2) + first.at(3, 1) * second.at(1, 2) + first.at(3, 2) * second.at(2, 2) + first.at(3, 3) * second.at(3, 2),
                            first.at(3, 0) * second.at(0, 3) + first.at(3, 1) * second.at(1, 3) + first.at(3, 2) * second.at(2, 3) + first.at(3, 3) * second.at(3, 3)});
}

template <class Type>
vec3_T<Type> operator*(const mat<Type, 4, 4> &mat, const vec3_T<Type> &in) {
    return vec3_T<Type>(
        mat.at(0, 0) * in.x + mat.at(0, 1) * in.y + mat.at(0, 2) * in.z,
        mat.at(1, 0) * in.x + mat.at(1, 1) * in.y + mat.at(1, 2) * in.z,
        mat.at(2, 0) * in.x + mat.at(2, 1) * in.y + mat.at(2, 2) * in.z);
}

template <class Type>
vec3_T<Type> operator*(const mat<Type, 3, 3> &mat, const vec3_T<Type> &in) {
    return vec3_T<Type>(
        mat.at(0, 0) * in.x + mat.at(0, 1) * in.y + mat.at(0, 2) * in.z,
        mat.at(1, 0) * in.x + mat.at(1, 1) * in.y + mat.at(1, 2) * in.z,
        mat.at(2, 0) * in.x + mat.at(2, 1) * in.y + mat.at(2, 2) * in.z);
}

template <class Type>
vec4_T<Type> operator*(const mat<Type, 4, 4> &mat, const vec4_T<Type> &in) {
    return vec4_T<Type>(
        mat.at(0, 0) * in.x + mat.at(0, 1) * in.y + mat.at(0, 2) * in.z + mat.at(0, 3) * in.w,
        mat.at(1, 0) * in.x + mat.at(1, 1) * in.y + mat.at(1, 2) * in.z + mat.at(1, 3) * in.w,
        mat.at(2, 0) * in.x + mat.at(2, 1) * in.y + mat.at(2, 2) * in.z + mat.at(2, 3) * in.w,
        mat.at(3, 0) * in.x + mat.at(3, 1) * in.y + mat.at(3, 2) * in.z + mat.at(3, 3) * in.w);
}

typedef mat<float, 4, 4> mat4f;
typedef mat<float, 3, 3> mat3f;
typedef mat<double, 4, 4> mat4d;
typedef mat<double, 3, 3> mat3d;
typedef mat<int, 4, 4> mat4i;
typedef mat<int, 3, 3> mat3i;
typedef mat<unsigned int, 4, 4> mat4u;
typedef mat<unsigned int, 3, 3> mat3u;

inline vec3 mat4mulvec3(const mat4f &m, const vec3 &v) {
    vec3 ret;

    ret.x = m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z;
    ret.y = m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z;
    ret.z = m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z;
    return std::move(ret);
}