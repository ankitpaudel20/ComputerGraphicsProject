#pragma once

//#include <utility>

#include "core.h"
#include "material.h"



struct entity;

template <class T>
struct drawable {

    std::vector<T> m_vertices;
    std::vector<uint32_t> m_indices;
    uint32_t m_primitve = GL_TRIANGLES;
    Material material;
    bool doLightCalculations = true;
    std::string name;
    bool draw = true;

    glm::mat4 matModel = glm::mat4(1.0);

    drawable() = default;

    drawable(const std::vector<T> &vertices, std::vector<unsigned> indices, const std::string &n, std::vector<std::string> tex = std::vector<std::string>()) : name(n), m_vertices(vertices), m_indices(indices) {
        if (!tex.empty()) {
            material.ambientMap = tex[0];
            material.diffuseMap = tex[1];
            material.specularMap = tex[2];
            material.normalMap = tex[3];
        }
    }

    void delpos(const vec3 &delta) {
        translation = glm::translate(this->translation, (glm::vec3)delta);
        refreshModel();
    }

    void setpos(const vec3 &delta) {
        translation = glm::translate(glm::mat4(1), (glm::vec3)(delta));
        refreshModel();
    }

    glm::mat4 getTranslation() { return translation; }

    void setScale(const vec3 &scale) {
        scaling = glm::scale(glm::mat4(1.0), (glm::vec3)scale);
        refreshModel();
    }

    void setRotation(float angle, const vec3 &axis) {
        rotation = glm::rotate(rotation, glm::radians(angle), (glm::vec3)axis);
        refreshModel();
    }

    glm::mat4 *refreshModel() {
        matModel = translation * rotation * scaling;
        return &matModel;
    }

    virtual void userSpecificFunction() {}

  protected:
    glm::mat4 scaling = glm::mat4(1.0);
    glm::mat4 translation = glm::mat4(1.0);
    glm::mat4 rotation = glm::mat4(1.0);
};

typedef drawable<Vertex> Mesh;

struct line : public drawable<Vertex> {
    uint32_t m_width = 1;

    line(const std::vector<Vertex> &vertices, const std::string &n, std::vector<uint32_t> indices = std::vector<uint32_t>()) : drawable<Vertex>(vertices, indices, n) {
        doLightCalculations = false;
        m_primitve = GL_LINES;
        if (indices.empty()) {
            m_indices.reserve(vertices.size());
            for (uint32_t i = 0; i < vertices.size(); i++) {
                m_indices.emplace_back(i);
            }
        }
    }

    line() {}

    virtual void userSpecificFunction() {
        glLineWidth(m_width);
    }
};
