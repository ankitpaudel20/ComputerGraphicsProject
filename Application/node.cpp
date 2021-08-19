#include "node.h"
#include "drawable.h"
#include "transformations.h"
#include "engine.h"


void node::delpos(const vec3 &delta) {
    translation = trans::translate((delta)) * translation;
    refreshModel();
}

void node::setpos(const vec3 &position) {
    translation = trans::translate(position);
    refreshModel();
}

void node::setScale(const vec3 &scale) {
    scaling = trans::scaling3d(scale);
    refreshModel();
}

void node::setRotation(float angle, const vec3 &axis) {
    rotation = trans::rotation(angle * radian, axis) * rotation;
    refreshModel();
}

const mat4f &node::refreshModel() {
    matModel = translation * rotation * scaling;
    return matModel;
}

void node::draw(engine &graphicsEngine, mat4f matModel_in) {
    matModel_in = matModel * matModel_in;
    for (auto &mesh : meshes) {
        if (mesh->draw) {
            // graphicsEngine.doLightCalculations = mesh->doLightCalculations;
            graphicsEngine.currentMaterial = &mesh->material;
            graphicsEngine.drawTrianglesRasterized(mesh->m_vertices, mesh->m_indices, mesh->matModel * matModel_in);
        }
    }
    for (auto &child : children) {
        child.second->draw(graphicsEngine, matModel_in);
    }
}