#include "core.h"
#include "transformations.h"

struct node {
    //node* children=nullptr;
    //uint32_t nosChildren=0;
    std::map<std::string, node *> children;
    std::vector<drawable<Vertex> *> meshes;
    mat4 matModel = mat4(1.0);
    std::string shaderName;

    void delpos(const vec3 &delta) {
        translation = trans::translation((delta))*translation;
        refreshModel();
    }

    void setpos(const vec3 &position) {
        translation = trans::translation(position);
        refreshModel();
    }

    void setScale(const vec3 &scale) {
        scaling = trans::scaling3d(scale);
        refreshModel();
    }

    void setRotation(float angle, const vec3 &axis) {
        rotation = trans::rotation(angle*radian, axis)*rotation;
        refreshModel();
    }

    const mat4 &refreshModel() {
        matModel = translation * rotation * scaling;
        return matModel;
    }

  private:
    mat4 scaling =      mat4(1.0);
    mat4 translation =  mat4(1.0);
    mat4 rotation =     mat4(1.0);
};