#pragma once

#include "core.h"
#include "node.h"

struct pointLight {
  private:
    vec3 position;
    color diffuseColor;
    color ambientColor = color(255);
    node *model = nullptr;

  public:
    float intensity;
    float radius = 11;

    // float constant = 1;
    // float linear = 0.09;
    // float quadratic = 0.032;
    float constant = 0.85;
    float linear = 1.0;
    float quadratic = 0.06;

    pointLight(const vec3 &pos, const float &intensity, const color &diffcol = color(255)) : position(pos), diffuseColor(diffcol), intensity(intensity), ambientColor(diffcol) {}

    void delpos(const vec3 &delta) {
        position += delta;
        if (model) {
            model->delpos(delta);
        }
    }

    void setpos(const vec3 &newPos) {
        position = newPos;
        if (model) {
            model->setpos(newPos);
        }
    }

    void setmodel(node *model) {
        this->model = model;
        this->model->setpos(position);
    }

    void setdiffColor(const color &color) {
        diffuseColor = color;
        if (model) {
            for (auto &mesh : model->meshes) {
                mesh->material.diffuseColor = color;
                mesh->material.specularColor = color;
            }
        }
    }

    void setColor(const vec3 &color) {
        ambientColor = color;
        setdiffColor(color);
    }

    [[nodiscard]] inline const vec3 &getpos() const { return position; }
    [[nodiscard]] inline const color &get_diffuse_color() const { return diffuseColor; }
    [[nodiscard]] inline const color &get_ambient_color() const { return ambientColor; }
    [[nodiscard]] inline node *&getModel() { return model; }
};
