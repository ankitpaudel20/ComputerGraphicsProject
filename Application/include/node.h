#pragma once
#include "core.h"
#include <unordered_map>
#include <vector>

template <class T>
class drawable;

class engine;

/**
 * anything that can be transformed( has modelmatrix )
 */
struct node {
    std::unordered_map<std::string, node *> children;
    std::vector<drawable<Vertex> *> meshes;
    mat4f matModel = mat4f();

    void delpos(const vec3 &delta);

    void setpos(const vec3 &position);

    void setScale(const vec3 &scale);

    void setRotation(float angle, const vec3 &axis);

    const mat4f &refreshModel();

    void draw(engine &graphicsEngine, mat4f matModel_in = mat4f());

  private:
    mat4f scaling = mat4f();
    mat4f translation = mat4f();
    mat4f rotation = mat4f();
};