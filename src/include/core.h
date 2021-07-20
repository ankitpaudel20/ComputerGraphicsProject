#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "maths.h"
//#include <cassert>
//#include <ctime>
//#include <iostream>
//#include <map>
//#include <ostream>
//#include <string>
//#include <utility>
//#include <vector>

#ifdef _WIN32
const std::string pathDelemeter(std::string("\\") + "\\");
#else
const std::string pathDelemeter("/");
#endif // _WIN32

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

//#define NEWRENDERMETHOD

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
    vec3 position = vec3(0);
    vec3 normal = vec3(0);
    vec2 texCoord = vec2(0);

    Vertex(const vec3 &pos, const vec3 &nor = vec3(0), const vec2 &texcoord = vec2(0))
        : position(pos), normal(nor), texCoord(texcoord) {}
    Vertex() {}

    Vertex operator-(const Vertex &in) const {
        return Vertex(position - in.position, normal - in.normal, texCoord - in.texCoord);
    }
    Vertex operator+(const Vertex &in) const {
        return Vertex(position + in.position, normal + in.normal, texCoord + in.texCoord);
    }
    Vertex operator*(const float &in) const {
        return Vertex(position * in, normal * in, texCoord * in);
    }
};

struct dirLight {
    vec3 direction;
    float intensity;
    vec3 color;

    dirLight(const vec3 &dir = vec3::normalize(vec3(1)), const float &intensity = 1, const vec3 &diffcol = vec3(1)) : direction(dir), color(diffcol), intensity(intensity) {}
};
