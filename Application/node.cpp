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

static vec3 getCenterOfMass(const Vertex *vertices, const unsigned int size) {
    vec3 center(0);
    for (unsigned int i = 0; i < size; i++) {
        center += vertices[i].position;
    }
    return center / size;
}

void node::draw(engine &graphicsEngine, mat4f matModel_in) {
    matModel_in = matModel * matModel_in;
    for (auto &mesh : meshes) {
        //printf("model matrix: \n");
        //(mesh->matModel * matModel_in).print();
        graphicsEngine.currentMesh = mesh;
        graphicsEngine.makeRequiredTriangles(mesh->m_vertices, mesh->m_indices, matModel_in);
    }
    //for (auto &child : children) {
    //    child.second->draw(graphicsEngine, matModel_in);
    //}
}