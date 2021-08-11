#pragma once

#include "core.h"
#include "material.h"
#include "transformations.h"

/**
 * @brief this class represents all drawable entities that has a material and vertices
 */
template <class T>
struct drawable {

    std::vector<T> m_vertices;
    std::vector<uint32_t> m_indices;
    Material material;
    bool doLightCalculations = true;
    std::string name;
    bool draw = true;

    mat4f matModel = mat4f();

    drawable() = default;

    drawable(const std::vector<T> &vertices, std::vector<unsigned> indices, const std::string &n) : name(n), m_vertices(vertices), m_indices(indices) {
    }

    void delpos(const vec3 &delta) {
        translation = trans::translate(delta) * this->translation;
        refreshModel();
    }

    void setpos(const vec3 &delta) {
        translation = trans::translate(delta);
        refreshModel();
    }

    mat4f getTranslation() { return translation; }

    void setScale(const vec3 &scale) {
        scaling = trans::scaling3d(scale);
        refreshModel();
    }

    void setRotation(float angle, const vec3 &axis) {
        rotation = trans::rotation(radian * angle, axis);
        refreshModel();
    }

    mat4f *refreshModel() {
        matModel = translation * rotation * scaling;
        return &matModel;
    }

    virtual void userSpecificFunction() {}

  protected:
    mat4f scaling = mat4f();
    mat4f translation = mat4f();
    mat4f rotation = mat4f();
};

typedef drawable<Vertex> Mesh;
