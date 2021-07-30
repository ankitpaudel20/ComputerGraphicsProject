#pragma once
#include "core.h"
#include "transformations.h"
#include <unordered_map>
#include "drawable.h"


////
// anything that can be transformed( has modelmatrix )
////
struct node
{
    //node* children=nullptr;
    //uint32_t nosChildren=0;
    std::unordered_map<std::string, node *> children;
    std::vector<Mesh *> meshes;
    mat4f matModel = mat4f();
    // std::string shaderName;

    void delpos(const vec3 &delta)
    {
        translation = trans::translate((delta)) * translation;
        refreshModel();
    }

    void setpos(const vec3 &position)
    {
        translation = trans::translate(position);
        refreshModel();
    }

    void setScale(const vec3 &scale)
    {
        scaling = trans::scaling3d(scale);
        refreshModel();
    }

    void setRotation(float angle, const vec3 &axis)
    {
        rotation = trans::rotation(angle * radian, axis) * rotation;
        refreshModel();
    }

    const mat4f &refreshModel()
    {
        matModel = translation * rotation * scaling;
        return matModel;
    }

private:
    mat4f scaling = mat4f();
    mat4f translation = mat4f();
    mat4f rotation = mat4f();
};