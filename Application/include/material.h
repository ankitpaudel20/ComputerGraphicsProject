//
// Created by ankit on 5/9/2021.
//
#pragma once

#include "core.h"
struct texture {
    unsigned char* m_data=nullptr;

    int w = 0, h = 0, m_bpp;
    ~texture() { delete[] m_data; }

};

struct Material {
    int id = -1;
    float AmbientStrength = 0.7;
    float DiffuseStrength = 1;
    float SpecularStrength = 1;
    color specularColor = color(255);
    float shininess = 32;
    color diffuseColor = color(255);
    texture diffuse;
    texture specular;
    /*  std::string ambientMap;
    std::string diffuseMap;
    std::string specularMap;
    std::string normalMap;*/
    Material() {}
};
