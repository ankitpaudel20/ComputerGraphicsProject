#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "maths.h"

#include <filesystem>
namespace fs = std::filesystem;

#ifdef _WIN32
const std::string pathDelemeter(std::string("\\") + "\\");
#else
const std::string pathDelemeter("/");
#endif // _WIN32

#ifdef _MSC_VER

#ifdef _DEBUG
#define MDEBUG
#else
#define DEBUG_BREAK
#endif

#else

#ifndef NDEBUG
#define DEBUG_BREAK __builtin_trap()
#define MDEBUG
#else
#define DEBUG_BREAK
#endif

#endif

#ifdef MDEBUG
#define GLcall_P(y, x) \
    GLClearError();    \
    y = x;             \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#define GLcall(x)   \
    GLClearError(); \
    x;              \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLcall(x) x
#define GLcall_P(y, x) y = x
#endif

#define NEWRENDERMETHOD
#define PHONG_SHADING

#define EXTRA_VARIABLE_TYPE vec3

inline void GLClearError() {
    while (glGetError() != GL_NO_ERROR)
        ;
}

inline bool GLLogCall(const char *function, const char *file, int line) {
    while (GLenum errorcode = glGetError()) {
        std::string error;
        switch (errorcode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << "[Opengl Error] ( Error code :" << errorcode << " : " << error << " )" << function << " " << file << " : " << line << std::endl;
        return false;
    }
    return true;
}

//#define _DEBUG

#define ASSERT(x)      \
    if (!(x)) {        \
        DEBUG_BREAK;   \
        assert(false); \
    }

typedef unsigned int uint;
inline vec3 operator*(glm::mat4 mat, vec3 vec) {
    glm::vec4 temp(vec.x, vec.y, vec.z, 1);
    temp = mat * temp;
    return vec3(temp.x, temp.y, temp.z);
}

struct Vertex {
    vec4 position = vec4(0);
    vec3 normal = vec3(0);
    vec2 texCoord = vec2(0);

    Vertex(const vec4 &pos, const vec3 &nor = vec3(0), const vec2 &texcoord = vec2(0)) : position(pos), normal(nor), texCoord(texcoord) {}
    Vertex() {}

    Vertex operator*(const float &f) const {
#ifdef PHONG_SHADING
        return Vertex(position * f, normal * f, texCoord * f);
#else
        return Vertex(position * f, normal, texCoord * f);
#endif
    }
    Vertex operator+(const Vertex &f) const {
#ifdef PHONG_SHADING
        return Vertex(f.position + position, f.normal + normal, f.texCoord + texCoord);
#else
        return Vertex(f.position + position, normal, f.texCoord + texCoord);
#endif
    }
    Vertex operator-(const Vertex &f) const {
#ifdef PHONG_SHADING
        return Vertex(position - f.position, normal - f.normal, texCoord - f.texCoord);
#else
        return Vertex(position - f.position, normal, texCoord - f.texCoord);
#endif
    }
    Vertex perspectiveMul(const mat4f &per) {
        position = per * position;
        texCoord /= (fabs(position.w) < epsilon ? epsilon : position.w);
        position = vec4(position.x / (fabs(position.w) < epsilon ? epsilon : position.w), position.y / (fabs(position.w) < epsilon ? epsilon : position.w), 1 / position.z, 1);
        // position = position / (fabs(position.w) < epsilon ? epsilon : position.w);
        return *this;
    }

    inline static Vertex perspectiveMul(Vertex in, const mat4f &per) {
        return in.perspectiveMul(per);
    }
};

struct color {
    uint32_t col;
    color() : color(0) {}
    color(uint8_t r) : color(r, r, r) {}
    color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : col((a << 24u) | (b << 16u) | (g << 8u) | r) {}
    color(const vec3 &in) : color(in.x * 255, in.y * 255, in.z * 255) {}

    vec4 getcolor() {
        auto temp = (uint8_t *)&col;
        return vec4(temp[0], temp[1], temp[2], temp[3]) / 255;
    }
    inline uint8_t &r() {
        auto temp = (uint8_t *)&col;
        return temp[0];
    }
    inline uint8_t &g() {
        auto temp = (uint8_t *)&col;
        return temp[1];
    }
    inline uint8_t &b() {
        auto temp = (uint8_t *)&col;
        return temp[2];
    }
    inline uint8_t &a() {
        auto temp = (uint8_t *)&col;
        return temp[3];
    }

    color operator*(const color &in) const {
        auto temp = (uint8_t *)&col;
        auto temp1 = (uint8_t *)&in.col;
        auto r = ((unsigned short)temp[0] * temp1[0]) / 255u;
        auto g = ((unsigned short)temp[1] * temp1[1]) / 255u;
        auto b = ((unsigned short)temp[2] * temp1[2]) / 255u;
        auto a = ((unsigned short)temp[3] * temp1[3]) / 255u;
        color ret(r, g, b, a);
        return std::move(ret);
    }

    void operator*=(const color &in) {
        auto temp = (uint8_t *)&col;
        auto temp1 = (uint8_t *)&in.col;
        temp[0] = ((unsigned short)temp[0] * temp1[0]) / 255u;
        temp[1] = ((unsigned short)temp[1] * temp1[1]) / 255u;
        temp[2] = ((unsigned short)temp[2] * temp1[2]) / 255u;
        temp[3] = ((unsigned short)temp[3] * temp1[3]) / 255u;
    }

    color operator+(const color &in) const {
        auto temp = (uint8_t *)&col;
        auto temp1 = (uint8_t *)&in.col;
        unsigned short r = (unsigned short)temp[0] + temp1[0];
        unsigned short g = (unsigned short)temp[1] + temp1[1];
        unsigned short b = (unsigned short)temp[2] + temp1[2];
        unsigned short a = (unsigned short)temp[3] + temp1[3];
        return color(r > 255 ? 255 : r, g > 255 ? 255 : g, b > 255 ? 255 : b, a > 255 ? 255 : a);
    }

    void operator+=(const color &in) {
        auto temp = (uint8_t *)&col;
        auto temp1 = (uint8_t *)&in.col;
        unsigned short r = (unsigned short)temp[0] + temp1[0];
        unsigned short g = (unsigned short)temp[1] + temp1[1];
        unsigned short b = (unsigned short)temp[2] + temp1[2];
        unsigned short a = (unsigned short)temp[3] + temp1[3];
        temp[0] = r > 255 ? 255 : r;
        temp[1] = g > 255 ? 255 : g;
        temp[2] = b > 255 ? 255 : b;
        temp[3] = a > 255 ? 255 : a;
    }

    color operator*(const float &in) const {
        auto temp = (uint8_t *)&col;
        unsigned short r = (unsigned short)temp[0] * in;
        unsigned short g = (unsigned short)temp[1] * in;
        unsigned short b = (unsigned short)temp[2] * in;
        return color(r > 255 ? 255 : r, g > 255 ? 255 : g, b > 255 ? 255 : b);
    }

    void operator*=(const float &in) {
        auto temp = (uint8_t *)&col;
        unsigned short r = (unsigned short)temp[0] * in;
        unsigned short g = (unsigned short)temp[1] * in;
        unsigned short b = (unsigned short)temp[2] * in;
        temp[0] = r > 255 ? 255 : r;
        temp[1] = g > 255 ? 255 : g;
        temp[2] = b > 255 ? 255 : b;
    }

    bool operator==(const color &in) { return col == in.col; }
};

std::string searchRes() {
    auto currentPath = fs::current_path();

    auto orgiPath = currentPath;
    bool resPathFound = false, appPathFound = false;

    while (true) {
        if (!resPathFound && !appPathFound && currentPath.has_parent_path()) {
            fs::current_path(currentPath.parent_path());
            currentPath = fs::current_path();
        } else if (appPathFound) {
            try {
#ifdef _WIN32
                auto a = currentPath.string().append("\\res");
#else
                auto a = currentPath.string().append("/res");
#endif
                fs::current_path(a);
                fs::current_path(orgiPath);
                return a;
            } catch (const std::exception &e) {
                std::cout << e.what() << "\n \"res\" folder not found inside \"Application\" folder \n";
                return std::string();
            }
        } else
            break;

        //std::cout << "currently in path: " << currentPath.string() << std::endl;

        for (auto &dirs : fs::directory_iterator(currentPath)) {
            if (dirs.is_directory() && dirs.path().filename().string() == "Application") {
                appPathFound = true;
                break;
            }
        }
    }

    std::cout << "folder \"Application\" not found in 4 back iterations" << std::endl;
    DEBUG_BREAK;
    return std::string();
}

struct dirLight {
    vec3 direction;
    float intensity;
    color col;

    dirLight(const vec3 &dir = vec3::normalize(vec3(1)), const float &intensity = 1, const color &diffcol = color(255)) : direction(dir), col(diffcol), intensity(intensity) {}
};