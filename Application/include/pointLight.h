#pragma once

#include "core.h"
#include "node.h"
#include "drawable.h"
/**
 * @brief pointlight
 */
struct pointLight : public node {
  private:
    vec3 position;
    color diffuseColor;
    color ambientColor = color(255);

  public:
    float intensity;

    float constant = 1;
    float linear = 0.09;
    float quadratic = 0.032;
    //    float constant = 0.85;
    //    float linear = 1.0;
    //    float quadratic = 0.06;

    pointLight(const vec3 &pos, const float &intensity, Mesh *mesh = nullptr, const color &diffcol = color(255)) : position(pos), diffuseColor(diffcol), intensity(intensity), ambientColor(diffcol) {
        if (mesh) {
            meshes.emplace_back(mesh);
            node::setpos(pos);
            mesh->doLightCalculations = false;
        }
    }

    void delpos(const vec3 &delta) {
        position += delta;
        node::delpos(delta);
    }

    void setpos(const vec3 &newPos) {
        position = newPos;
        node::setpos(newPos);
    }

    //    void setmodel(node *model_in) {
    //        model = model_in;
    //        model->setpos(position);
    //        if (!model->meshes.empty()){
    //            model->meshes[0]->doLightCalculations=false;
    //        }
    //    }

    void setdiffColor(const color &color) {
        diffuseColor = color;
        for (auto &mesh : node::meshes) {
            mesh->material.diffuseColor = color;
            mesh->material.specularColor = color;
        }
    }

    void setColor(const vec3 &col) {
        ambientColor = color(col);
        setdiffColor(ambientColor);
    }

    [[nodiscard]] inline const vec3 &getpos() const { return position; }
    [[nodiscard]] inline const color &get_diffuse_color() const { return diffuseColor; }
    [[nodiscard]] inline const color &get_ambient_color() const { return ambientColor; }
};
