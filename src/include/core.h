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

//#define _DEBUG

#define ASSERT(x)                                                              \
    if (!(x)) {                                                                \
        DEBUG_BREAK;                                                           \
        assert(false);                                                         \
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
    vec3 tangent = vec3(0);
    vec3 bitangent = vec3(0);

    Vertex(const vec3 &pos, const vec3 &nor = vec3(0),
           const vec2 &texcoord = vec2(0), const vec3 &tangent = vec3(0),
           const vec3 &bitangent = vec3(0), const float texid = 0)
        : position(pos), normal(nor), tangent(tangent), bitangent(bitangent),
          texCoord(texcoord) {}
    Vertex() {}
};

struct dirLight {
    vec3 direction;
    float intensity;
    vec3 color;

    dirLight(const vec3 &dir = vec3::normalize(vec3(1)),
             const float &intensity = 1, const vec3 &diffcol = vec3(1))
        : direction(dir), color(diffcol), intensity(intensity) {}
};
