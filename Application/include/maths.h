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

constexpr float radian = 3.14159265 / 180;
constexpr float epsilon = 1e-4f;
constexpr float pi = 3.14159265;

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
        return vec2_T<T> (x * b.x , y * b.y);
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
        return vec3_T<T> (x * b.x , y * b.y, z * b.z);
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
    size_t xdim = x_dim, ydim = y_dim; // xdim=no of cols   ydim= no of rows

  public:
    mat() {
        for (int i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                operator()(i, j) = i == j ? 1 : 0;
    }

    mat(const Type &in) {
        for (int i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                operator()(i, j) = in;
    }

    mat(const std::array<Type, x_dim * y_dim> &in) {
        assert(in.size() == x_dim * y_dim);
        m_data = in;
    }

    
    template <class U>
    mat(const mat<U, x_dim, y_dim> &in) {
        for (int i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                operator()(i, j) = in(i, j);
    }

    Type &operator()(const size_t &r, const size_t &c) {
        assert(r >= 0 && r < y_dim && c >= 0 && c < x_dim);
        return m_data[r * xdim + c];
    }

    const Type &operator()(const size_t &r, const size_t &c) const {
        assert(r >= 0 && r < y_dim && c >= 0 && c < x_dim);
        return m_data[r * xdim + c];
    }

    void print() const {
        for (size_t i = 0; i < y_dim; i++) {
            printf("(");
            for (size_t j = 0; j < x_dim; j++)
                printf("%8.3lf", operator()(i, j));
            printf(")\n");
        }
        // printf("\n");
    }

    mat<Type, x_dim, y_dim> toUpper() {
        mat<Type, x_dim, y_dim> temp = *this;
        size_t i, j, k;
        Type constant;
        for (i = 0; i < temp.ydim; i++) {
            constant = temp(i, i);
            for (j = i + 1; j < temp.ydim; j++) {
                constant = temp(j, i) / temp(i, i);
                for (k = i; k < temp.xdim; k++)
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

        for (i = 0; i < ydim; i++)
            for (j = 0; j < xdim; j++)
                inv(i, j) = ((i == j) ? 1 : 0);

        Type constant;
        // temp.print(inv);
        // std::cout << std::endl;

        for (i = 0; i < ydim; ++i) {
            if (temp(i, i) == 0) {
                for (j = i; j < ydim; j++) {
                    if (temp(j, j) != 0) {
                        // printf("swapping %d row with %d row\n", i, j);
                        auto dest = &temp(j, 0);
                        auto src = &temp(i, 0);

                        auto *swaptemp = new Type[xdim];

                        memcpy(swaptemp, dest, xdim * sizeof(Type));
                        memcpy(dest, src, xdim * sizeof(Type));
                        memcpy(src, swaptemp, xdim * sizeof(Type));

                        dest = &inv(j, 0);
                        src = &inv(i, 0);

                        memcpy(swaptemp, dest, xdim * sizeof(Type));
                        memcpy(dest, src, xdim * sizeof(Type));
                        memcpy(src, swaptemp, xdim * sizeof(Type));

                        // temp.print(inv);
                        // std::cout << std::endl;

                        delete[] swaptemp;
                    }
                }
            }

            constant = temp(i, i);
            for (j = i; j < xdim; j++)
                temp(i, j) /= constant;
            for (j = 0; j < xdim; j++)
                inv(i, j) /= constant;

            // printf("normalizing %zu th diagonal\n\n", i);
            // temp.print(inv);
            // std::cout << std::endl;

            for (j = 0; j < ydim; j++) {
                if (j == i)
                    continue;
                constant = temp(j, i);
                // printf("R%zu = R%zu - %lf * R%zu \n\n", j, j, constant, i);
                for (k = i; k < xdim; k++) {
                    // printf("%.3lf = %.3lf - %.3f * %.3lf\n", temp(j, k), temp(j, k),
                    // temp(i, k), constant);
                    temp(j, k) = temp(j, k) - temp(i, k) * constant;
                }

                for (k = 0; k < xdim; k++) {
                    // printf("%.3lf = %.3lf - %.3lf * %.3lf\n", inv(j, k), inv(j, k),
                    // inv(i, k), constant);
                    inv(j, k) = inv(j, k) - inv(i, k) * constant;
                }

                // std::cout << std::endl;
                // temp.print(inv);
                // std::cout << std::endl;
            }

            // printf("%d th row operation\n", i);
            // std::cout << temp << std::endl<< inv;
        }
        // printf("the inverse is\n");
        // std::cout << inv;
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
                ret(i, j) = in(i, j) + operator()(i, j);
        return ret;
    }

    mat<Type, x_dim, y_dim> operator-(const mat<Type, x_dim, y_dim> &in) {
        mat<Type, x_dim, y_dim> ret;
        for (size_t i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                ret(i, j) = in(i, j) - operator()(i, j);
        return ret;
    }

    void operator+=(const mat<Type, x_dim, y_dim> &in) {
        for (size_t i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                operator()(i, j) += in(i, j);
    }

    void operator-=(const mat<Type, x_dim, y_dim> &in) {
        for (size_t i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                operator()(i, j) -= in(i, j);
    }

    void operator*=(const mat<Type, x_dim, y_dim> &in) {
        assert(x_dim == y_dim);
        std::array<Type, x_dim * y_dim> new_buffer;
        int i, j, k;
        for (i = 0; i < y_dim; i++) {
            for (j = 0; j < y_dim; j++) {
                new_buffer[i * y_dim + j] = 0;
                for (k = 0; k < y_dim; k++)
                    new_buffer[i * y_dim + j] += operator()(i, k) * in(k, j);
            }
        }
        m_data = std::move(new_buffer);
    }

    void operator*=(const Type &a) {
        for (size_t i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                operator()(i, j) *= a;
    }

    void operator/=(const Type &a) {
        for (size_t i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                operator()(i, j) /= a;
    }

    template <size_t lx, size_t ly>
    mat<Type, x_dim, ly> operator*(const mat<Type, lx, ly> &in) const {
        assert(x_dim == ly);
        mat<Type, x_dim, ly> ret;
        int i, j, k;
        for (i = 0; i < y_dim; i++) {
            for (j = 0; j < lx; j++) {
                ret(i, j) = 0;
                for (k = 0; k < x_dim; k++)
                    ret(i, j) += operator()(i, k) * in(k, j);
            }
        }
        return ret;
    }

    vec3_T<Type> operator*(const vec3_T<Type> &in) const {
        // assert(x_dim == 3 && y_dim == 3);
        if (x_dim != 3 || y_dim != 3) {
            DEBUG_BREAK;
        }

        vec3_T<Type> ret;
        ret.x = operator()(0, 0) * in.x + operator()(0, 1) * in.y + operator()(0, 2) * in.z;
        ret.y = operator()(1, 0) * in.x + operator()(1, 1) * in.y + operator()(1, 2) * in.z;
        ret.z = operator()(2, 0) * in.x + operator()(2, 1) * in.y + operator()(2, 2) * in.z;
        return ret;
    }

    vec2_T<Type> operator*(const vec2_T<Type> &in) const {
        assert(x_dim == 2 && y_dim == 2);
        vec2_T<Type> ret;
        ret.x = operator()(0, 0) * in.x + operator()(0, 1) * in.y;
        ret.y = operator()(1, 0) * in.x + operator()(1, 1) * in.y;
        return ret;
    }

    vec4_T<Type> operator*(const vec4_T<Type> &in) const {
        assert(x_dim == 4 && y_dim == 4);
        vec4_T<Type> ret;
        ret.x = operator()(0, 0) * in.x + operator()(0, 1) * in.y + operator()(0, 2) * in.z + operator()(0, 3) * in.w;
        ret.y = operator()(1, 0) * in.x + operator()(1, 1) * in.y + operator()(1, 2) * in.z + operator()(1, 3) * in.w;
        ret.z = operator()(2, 0) * in.x + operator()(2, 1) * in.y + operator()(2, 2) * in.z + operator()(2, 3) * in.w;
        ret.w = operator()(3, 0) * in.x + operator()(3, 1) * in.y + operator()(3, 2) * in.z + operator()(3, 3) * in.w;
        return ret;
    }

    mat<Type, x_dim, y_dim> operator*(const Type &a) {
        mat<Type, x_dim, y_dim> ret;
        for (size_t i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                ret(i, j) = operator()(i, j) * a;
    }

    mat<Type, x_dim, y_dim> operator/(const Type &a) {
        mat<Type, x_dim, y_dim> ret;
        for (size_t i = 0; i < y_dim; i++)
            for (size_t j = 0; j < x_dim; j++)
                ret(i, j) = operator()(i, j) / a;
    }

    Type determinant() {
        // std::cout << "original " << *this;
        assert(x_dim == y_dim);
        mat<Type, x_dim, y_dim> temp = toUpper();
        // std::cout << "converted to upper" << temp;
        Type ret = 1;
        for (size_t i = 0; i < temp.ydim; i++)
            ret *= temp(i, i);

        return ret;
    }

    vec2_T<size_t> dimensions() const { return vec2_T<size_t>(xdim, ydim); }
};

typedef mat<float, 4, 4> mat4f;
typedef mat<float, 3, 3> mat3f;
typedef mat<float, 4, 4> mat4;
typedef mat<float, 3, 3> mat3;

inline vec3 mat4mulvec3(const mat4 &m, const vec3 &v) {
    vec3 ret;

    ret.x = m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z;
    ret.y = m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z;
    ret.z = m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z;
    return std::move(ret);
}