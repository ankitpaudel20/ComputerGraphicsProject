#pragma once
#include "core.h"
#include <functional>
#include <algorithm>
#include <limits>
#include <functional>

#include "material.h"
#include "pointLight.h"
#include "camera.h"
#include "thread_pool.hpp"

inline static int roundfloat(const float &in) {
    return in < 0 ? in - 0.5f : in + 0.5f;
}

inline static int rounddouble(const double &in) {
    return in < 0 ? in - 0.5f : in + 0.5f;
}

class Vertex2;
/**
 * @brief Framebuffer implementation for cpu
 */
struct framebuffer {
    std::vector<std::tuple<Vertex2 *, Mesh *>> fragments;
    float *z;
    color *colorlayer;

    float *clearedz;
    color *clearedcolorlayer;
    size_t x_size, y_size;
    int xmax, xmin, ymax, ymin;
    framebuffer(const size_t &x, const size_t &y) : x_size(x), y_size(y) {
        fragments.reserve(x_size * y_size);
        colorlayer = new color[x_size * y_size];
        z = new float[x_size * y_size];

        clearedcolorlayer = new color[x_size * y_size];
        clearedz = new float[x_size * y_size];
        for (uint32_t x = 0; x < x_size * y_size; ++x) {
            clearedcolorlayer[x] = color(0, 0, 0, 255);
            clearedz[x] = std::numeric_limits<float>::max();
        }

        xmax = x_size / 2;
        xmin = -xmax;
        ymax = y_size / 2;
        ymin = -ymax;

        clear();
    }

    ~framebuffer() {
        delete[] colorlayer;
        delete[] z;
    }

    void clear() {
        memcpy(colorlayer, clearedcolorlayer, x_size * y_size * sizeof(color));
        memcpy(z, clearedz, x_size * y_size * sizeof(float));
    }
};

#ifdef PHONG_SHADING
#define EXTRA_VERTEX_INFO vec3
#else
#define EXTRA_VERTEX_INFO vec3_T<vec3>
#endif

/**
 * @brief Vertex + extra info required for shading
 */
struct Vertex2 {
    Vertex v;
    //fragment Position for phong and INTENSITY for gouraud
    EXTRA_VERTEX_INFO extraInfoAboutVertex;
    Vertex2(const Vertex &vin, const EXTRA_VERTEX_INFO &pos) : v(vin), extraInfoAboutVertex(pos) {}
    Vertex2() {}
    Vertex2 operator*(const float &f) const {
        return Vertex2(v * f, extraInfoAboutVertex * f);
    }
    Vertex2 operator+(const Vertex2 &f) const {
        return Vertex2(f.v + v, f.extraInfoAboutVertex + extraInfoAboutVertex);
    }
    Vertex2 operator-(const Vertex2 &f) const {
        return Vertex2(v - f.v, extraInfoAboutVertex - f.extraInfoAboutVertex);
    }
    void operator*=(const float &f) {
        v *= f;
        extraInfoAboutVertex *= f;
    }
    void operator/=(const float f) {
        v /= f;
        extraInfoAboutVertex /= f;
    }
};

/**
 * @brief backend for all rendering purposes
 */
struct engine {

    framebuffer *fboCPU;

    uint32_t vao, vbo, ibo, tex, shader;

  private:
    thread_pool pool;

    const char *vertexShaderSource = R"(#version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 texCoord;
    out vec2 f_texCoord;
    void main()
    {
       	gl_Position = vec4(aPos.xyz, 1.0);
	    f_texCoord=texCoord;
    }
    )";

    const char *fragmentShaderSource = R"(#version 330 core
    out vec4 FragColor;
    in vec2 f_texCoord;
    uniform sampler2D tex;
    void main()
    {
       FragColor = texture(tex,f_texCoord);
       //FragColor = vec4(f_texCoord.x,f_texCoord.y,0,0);            
       // if(FragColor.xyz==vec3(0)){
       //     //FragColor =vec4(FragColor.w);
       // }
    }
    )";

    float vertices[20] =
        {
            //    x      y      z
            -1.0f, -1.0f, -0.0f, 0.f, 0.f,
            1.0f, 1.0f, -0.0f, 1.f, 1.f,
            -1.0f, 1.0f, -0.0f, 0.f, 1.f,
            1.0f, -1.0f, -0.0f, 1.f, 0.f};

    uint32_t indices[6] =
        {
            //  2---,1
            //  | .' |
            //  0'---3
            0, 1, 2,
            0, 3, 1};

  public:
    /**
 * @brief constructor and opengl draw surface initialization
 */
    engine(const uint32_t &x, const uint32_t &y) {
        fboCPU = new framebuffer(x, y);

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ibo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        GLcall(glGenTextures(1, &tex));
        GLcall(glActiveTexture(GL_TEXTURE0));
        GLcall(glBindTexture(GL_TEXTURE_2D, tex));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GLcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboCPU->x_size, fboCPU->y_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
        GLcall(glBindTexture(GL_TEXTURE_2D, 0));

        // build and compile our shader program
        // ------------------------------------
        // vertex shader
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }
        // fragment shader
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }
        // link shaders
        shader = glCreateProgram();
        glAttachShader(shader, vertexShader);
        glAttachShader(shader, fragmentShader);
        glLinkProgram(shader);
        // check for linking errors
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                      << infoLog << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glUseProgram(shader);

        auto location = glGetUniformLocation(shader, "tex");
        if (location == -1)
            std::cout << "Warning: uniform tex does not exist !" << std::endl;
        glUniform1i(location, 0);
        glUseProgram(0);
    }

    /**
 * @brief destructor
 */
    ~engine() {
        delete fboCPU;
        GLcall(glDeleteBuffers(1, &vbo));
        GLcall(glDeleteVertexArrays(1, &vao));
        GLcall(glDeleteTextures(1, &tex));
        GLcall(glDeleteProgram(shader));
    }

    /**
 * @brief clear all drawing surface
 */
    void clear() {
        triangles.clear();
        glClear(GL_COLOR_BUFFER_BIT);
        fboCPU->clear();
    }

    /**
 * @brief send framebuffer from cpu to gpu as texture to be able to render using opengl
 */
    void draw() {
        trianglePostProcessor();
        rasterizeTriangles();

        const uint8_t *imageData = (uint8_t *)&fboCPU->colorlayer[0].col;
        glUseProgram(shader);
        GLcall(glBindTexture(GL_TEXTURE_2D, tex));
        GLcall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fboCPU->x_size, fboCPU->y_size, GL_RGBA, GL_UNSIGNED_BYTE, imageData));
        glBindVertexArray(vao);
        GLcall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
    }

    /**
 * @brief drawlines using given vertices and indices
 */
    void drawLines(const std::vector<vec2_T<int>> &points, const color &col = color(255), const std::vector<uint32_t> &indices = std::vector<uint32_t>()) {
        if (indices.empty()) {
            for (size_t i = 0; i < points.size(); i += 2) {
                draw_bresenham_adjusted(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y, col);
            }
            return;
        }

        for (size_t i = 0; i < indices.size(); i += 2) {
            draw_bresenham_adjusted(points[indices[i]].x, points[indices[i]].y, points[indices[i + 1]].x, points[indices[i + 1]].y, col);
        }
    }

    /**
 * @brief draw linestrip fron given set of vertices, indices and color
 */
    void drawLinestrip(const std::vector<vec2_T<int>> &points, const color &col = color(255), const std::vector<uint32_t> &indices = std::vector<uint32_t>()) {
        for (size_t i = 0; i < points.size(); i++) {
            draw_bresenham_adjusted(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y, col);
        }
    }

    /**
 * @brief set of triangles to be drawn to screen
 */
    std::vector<std::tuple<std::array<Vertex2, 3>, Mesh *>> triangles;

    /**
 * @brief switch for backface culling
 */
    bool cullBackface = true;

    /**
 * @brief which material to use for further render calls
 */
    Mesh *currentMesh = nullptr;

    /**
 * @brief lights.
 */
    float ambientLightIntensity = 0.1f;
    dirLight dirlight;
    std::vector<pointLight> pointLights;

    /**
 * @brief camera
 */
    camera *cam = nullptr;

    bool wireframe = false;

    /**
 * @brief essentially a fragment shader : inputs fragment position and information and outputs color value for the fragment
 */
    color getcolor(const Vertex2 &v, Mesh *mesh) const {
        color col;
        if (mesh->material.diffuse.w) {
            float intpart;
            int tx = roundfloat(std::modf(v.v.texCoord.x, &intpart) * (mesh->material.diffuse.w - 1));
            int ty = roundfloat(std::modf(v.v.texCoord.y, &intpart) * (mesh->material.diffuse.h - 1));
            auto ret = &mesh->material.diffuse.m_data[(abs(tx) + mesh->material.diffuse.w * abs(ty)) * mesh->material.diffuse.m_bpp];
            col = color(*ret, *(ret + 1), *(ret + 2));
        } else
            col = mesh->material.diffuseColor;

        if (!mesh->doLightCalculations) {
            return col;
        }
        const vec3 norm = vec3::normalize(v.v.normal);
        col = color(norm);
        // if (col.r() == 1) {
        //     DEBUG_BREAK;
        // }
        return col;

        color result;
#ifdef PHONG_SHADING
        const auto fragpos = v.extraInfoAboutVertex;
        const auto viewDir = (cam->eye - fragpos).normalize();
        for (auto &light : pointLights) {
            float dist = vec3::dist(fragpos, light.getpos());
            float int_by_at = light.intensity / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
            if (int_by_at > 0.01) {
                result += CalcPointLight(light, v.v.normal, fragpos, viewDir, col, int_by_at, &mesh->material);
            }
        }
//        result += CalcDirLight(v.v.normal * v.v.position.w, viewDir, col, &mesh->material);
#else
        result += col * (v.extraInfoAboutVertex.x * v.v.position.w);
        result += col * (v.extraInfoAboutVertex.y * v.v.position.w);
        result += color(v.extraInfoAboutVertex.z * v.v.position.w);
#endif
        result += col * ambientLightIntensity;

        return result;
    }

    bool printed = false;

    void trianglePostProcessor() {
#ifdef MULTITHREADED
        pool.parallelize_loop(0, triangles.size(), [&](const unsigned int start, const unsigned int end) {
            for (unsigned int i = start; i < end; i++) {
                auto &tris = triangles[i];
                std::array<Vertex2, 3> &vert = {std::get<0>(tris)};

                const auto invZ0 = 1 / vert[0].v.position.w;
                const auto invZ1 = 1 / vert[1].v.position.w;
                const auto invZ2 = 1 / vert[2].v.position.w;

                vert[0] *= invZ0;
                vert[1] *= invZ1;
                vert[2] *= invZ2;

                vert[0].v.position.w = invZ0;
                vert[1].v.position.w = invZ1;
                vert[2].v.position.w = invZ2;

                vert[0].v.position.x *= fboCPU->xmax;
                vert[0].v.position.y *= fboCPU->ymax;
                vert[1].v.position.x *= fboCPU->xmax;
                vert[1].v.position.y *= fboCPU->ymax;
                vert[2].v.position.x *= fboCPU->xmax;
                vert[2].v.position.y *= fboCPU->ymax;
            }
        });
#else
        for (auto &tris : triangles) {
            std::array<Vertex2, 3> &vert = {std::get<0>(tris)};

            const auto invZ0 = 1 / vert[0].v.position.w;
            const auto invZ1 = 1 / vert[1].v.position.w;
            const auto invZ2 = 1 / vert[2].v.position.w;

            vert[0] *= invZ0;
            vert[1] *= invZ1;
            vert[2] *= invZ2;

            vert[0].v.position.w = invZ0;
            vert[1].v.position.w = invZ1;
            vert[2].v.position.w = invZ2;

            vert[0].v.position.x *= fboCPU->xmax;
            vert[0].v.position.y *= fboCPU->ymax;
            vert[1].v.position.x *= fboCPU->xmax;
            vert[1].v.position.y *= fboCPU->ymax;
            vert[2].v.position.x *= fboCPU->xmax;
            vert[2].v.position.y *= fboCPU->ymax;
        }
#endif
    }

    static bool isCulled(std::array<Vertex2, 3> &t) {
        const float area = (t[0].v.position.x * t[1].v.position.y - t[1].v.position.x * t[0].v.position.y) / (t[0].v.position.w * t[1].v.position.w) +
                           (t[1].v.position.x * t[2].v.position.y - t[2].v.position.x * t[1].v.position.y) / (t[1].v.position.w * t[2].v.position.w) +
                           (t[2].v.position.x * t[0].v.position.y - t[0].v.position.x * t[2].v.position.y) / (t[2].v.position.w * t[0].v.position.w);

        return area > 0;
    }

    /**
 * @brief fill triangles array by doing required clipping and culling
 */
    void makeRequiredTriangles(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const mat4f &modelmat) {

        auto view = trans::lookAt(cam->eye, cam->eye + cam->getViewDir(), cam->getUp());
        auto perFOV = trans::perspFOV(cam->FOV, (float)fboCPU->x_size / fboCPU->y_size, cam->nearPoint, cam->farPoint);
        for (size_t i = 0; i < indices.size(); i += 3) {
            const Vertex &v0 = vertices[indices[i]];
            const Vertex &v1 = vertices[indices[i + 1]];
            const Vertex &v2 = vertices[indices[i + 2]];

#ifdef PHONG_SHADING
            std::array<EXTRA_VERTEX_INFO, 3> extraInfoAboutVertex{modelmat * v0.position, modelmat * v1.position, modelmat * v2.position};
#else
            std::array<EXTRA_VERTEX_INFO, 3> extraInfoAboutVertex;
            extraInfoAboutVertex[0].x = modelmat * points[0].position;
            extraInfoAboutVertex[1].x = modelmat * points[1].position;
            extraInfoAboutVertex[2].x = modelmat * points[2].position;
#endif

#ifndef PHONG_SHADING
            fillExtraInformationForGoraudShading(points[0], extraInfoAboutVertex[0]);
            fillExtraInformationForGoraudShading(points[1], extraInfoAboutVertex[1]);
            fillExtraInformationForGoraudShading(points[2], extraInfoAboutVertex[2]);
#endif

            std::array<Vertex2, 3> t;
            vec4 temp[3] = {(view * (modelmat * (v0.position))), (view * (modelmat * (v1.position))), (view * (modelmat * (v2.position)))};

            t[0] = Vertex2((Vertex(perFOV * temp[0], modelmat * v0.normal, v0.texCoord)), extraInfoAboutVertex[0]);
            t[1] = Vertex2((Vertex(perFOV * temp[1], modelmat * v1.normal, v1.texCoord)), extraInfoAboutVertex[1]);
            t[2] = Vertex2((Vertex(perFOV * temp[2], modelmat * v2.normal, v2.texCoord)), extraInfoAboutVertex[2]);

            if (
                t[0].v.position.x > (t[0].v.position.w) && t[0].v.position.y > (t[0].v.position.w) &&
                t[1].v.position.x > (t[1].v.position.w) && t[1].v.position.y > (t[1].v.position.w) &&
                t[1].v.position.x > (t[1].v.position.w) && t[1].v.position.y > (t[1].v.position.w)) {
                continue;
            }

            auto cull = isCulled(t);

            if (cullBackface && cull) {
                continue;
            }

            std::array<bool, 3> clip{false, false, false};

            if (t[0].v.position.z < 0) {
                clip[0] = true;
            }
            if (t[1].v.position.z < 0) {
                clip[1] = true;
            }
            if (t[2].v.position.z < 0) {
                clip[2] = true;
            }

            if (clip[0] && clip[1] && clip[2]) {
                continue;
            } else if (clip[0]) {
                if (clip[1]) {
                    clip2helper(t, 0, 1, 2);
                } else if (clip[2]) {
                    clip2helper(t, 2, 0, 1);
                } else {
                    const std::array<Vertex2, 3> newTriangle = clip1helper(t, 1, 2, 0);
                    triangles.emplace_back(std::make_tuple(newTriangle, currentMesh));
                }
            } else if (clip[1]) {
                if (clip[2]) {
                    clip2helper(t, 1, 2, 0);
                } else if (clip[0]) {
                    clip2helper(t, 0, 1, 2);
                } else {
                    const std::array<Vertex2, 3> newTriangle = clip1helper(t, 2, 0, 1);
                    triangles.emplace_back(std::make_tuple(newTriangle, currentMesh));
                }
            } else if (clip[2]) {
                if (clip[0]) {
                    clip2helper(t, 2, 0, 1);
                } else if (clip[1]) {
                    clip2helper(t, 1, 2, 0);
                } else {
                    const std::array<Vertex2, 3> newTriangle = clip1helper(t, 0, 1, 2);
                    triangles.emplace_back(std::make_tuple(newTriangle, currentMesh));
                }
            }

            triangles.emplace_back(std::make_tuple(t, currentMesh));
        }
    }

  private:
    static inline float max(const float &first, const float &second) {
        return first > second ? first : second;
    }

    static inline vec3 reflect(const vec3 &I, const vec3 &N) {
        return I - N * 2.0 * vec3::dot(N, I);
    }

    /**
 * @brief function to calculate effect of directional light in a fragment with color col
 */
    inline color CalcDirLight(const vec3 &normal, const vec3 &viewdir, const color &col, const Material *material) const {
        auto diff = max(vec3::dot(normal, -dirlight.direction), 0.0f);
        auto spec = pow(max(vec3::dot(viewdir, reflect(dirlight.direction, normal)), 0.0), material->shininess);
        color diffuse = col * dirlight.col * dirlight.intensity * material->DiffuseStrength * diff;
        color specular = dirlight.col * dirlight.intensity * material->SpecularStrength * spec;
        diffuse += specular;
        diffuse.a() = 255;
        return std::move(diffuse);
    }

    /**
 * @brief function to calculate effect of a point light in a fragment with color col
 */
    static color CalcPointLight(const pointLight &light, const vec3 &normal, const vec3 &fragPos, const vec3 &viewDir, const color &diffuseColor, float int_by_at, const Material *material) {
        const vec3 lightDir = vec3::normalize(light.getpos() - fragPos);
        const float diff = max(vec3::dot(normal, lightDir), 0.0);
        const vec3 halfwayDir = vec3::normalize(lightDir + viewDir);
        const float spec = pow(max(vec3::dot(normal, halfwayDir), 0.0), material->shininess);
        const color ambient = diffuseColor * light.get_ambient_color() * (material->AmbientStrength * int_by_at);
        const color diffuse = diffuseColor * light.get_diffuse_color() * (material->DiffuseStrength * diff * int_by_at);
        const color specular = light.get_diffuse_color() * (material->SpecularStrength * spec * int_by_at);
        // return (ambient + diffuse + specular);
        return (diffuse + specular);
    }

    std::mutex fboCPUMutex;

    /**
 * @brief putpixel assuming middle of screen to be the origin
 */
    void putpixel_adjusted(int x, int y, float z, const color &col = color(255)) {
        assert(x < fboCPU->xmax && x > fboCPU->xmin && y < fboCPU->ymax && y > fboCPU->ymin);
        const size_t indx = ((size_t)x + fboCPU->xmax) + ((size_t)y + fboCPU->ymax) * fboCPU->x_size;
        fboCPU->colorlayer[indx] = col;
        fboCPU->z[indx] = z;
    }

    /**
 * @brief get z value of specified pixel from framebuffer
 */
    inline float getpixelZ_adjusted(int x, int y) const {
        const bool test = ((size_t)x + fboCPU->xmax) < fboCPU->x_size && (x + fboCPU->xmax) >= 0 && ((size_t)y + fboCPU->ymax) < fboCPU->y_size && (y + fboCPU->ymax) >= 0;
        // if (test) {
        //     return fboCPU->z[((size_t)x + fboCPU->xmax) + ((size_t)y + fboCPU->ymax) * fboCPU->x_size];
        // } else {
        //     return std::numeric_limits<float>::max();
        // }
        assert(test);
        return fboCPU->z[((size_t)x + fboCPU->xmax) + ((size_t)y + fboCPU->ymax) * fboCPU->x_size];
    }

    /**
 * @brief bresenham line drawing function to draw wireframe of objects
 */
    void draw_bresenham_adjusted(int x1, int y1, int x2, int y2, const color &col = color(0)) {
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);

        int lx = x2 > x1 ? 1 : -1;
        int ly = y2 > y1 ? 1 : -1;

        int x = x1, y = y1;
        bool changed = false;

        if (dx <= dy) {
            changed = true;
            std::swap(dx, dy);
            std::swap(lx, ly);
            std::swap(x, y);
        }
        int p = 2 * dy - dx;
        for (int k = 0; k <= dx; k++) {
            if (!changed)
                putpixel_adjusted(x, y, 1, col);
            else
                putpixel_adjusted(y, x, 1, col);

            if (p < 0) {
                x += lx;
                p += 2 * dy;
            } else {
                x += lx;
                y += ly;
                p += 2 * dy - 2 * dx;
            }
        }
    }

    /**
 * @brief clips and perspective transforms input triangle vertices if two points are out of clip space
 */
    static void clip2helper(std::array<Vertex2, 3> &v, unsigned char idx1, unsigned char idx2, unsigned char rem) {
        const float u1 = -(v[idx1].v.position.z) / (v[rem].v.position.z - v[idx1].v.position.z);
        const float u2 = -(v[idx2].v.position.z) / (v[rem].v.position.z - v[idx2].v.position.z);

        v[idx1] = v[idx1] + (v[rem] - v[idx1]) * u1;
        v[idx2] = v[idx2] + (v[rem] - v[idx2]) * u2;
    }

    /**
 * @brief clips and perspective transforms input triangle vertices if one point is out of the clip space
 */
    static std::array<Vertex2, 3> clip1helper(std::array<Vertex2, 3> &v, unsigned char idx1, unsigned char idx2, unsigned char idx3) {
        const float u1 = -(v[idx3].v.position.z) / (v[idx1].v.position.z - v[idx3].v.position.z);
        const float u2 = -(v[idx3].v.position.z) / (v[idx2].v.position.z - v[idx3].v.position.z);

        auto old_idx3 = v[idx3];
        v[idx3] = v[idx3] + (v[idx1] - v[idx3]) * u1;

        return {v[idx2], old_idx3 + (v[idx2] - old_idx3) * u2, v[idx3]};
    }

    /**
 * @brief rasterize BottomFlatTriangle: v2 is top unique and v1.x > v0.x
 */
    void fillBottomFlatTriangle(const Vertex2 &v0, const Vertex2 &v1, const Vertex2 &v2, Mesh *mesh) {
        assert(fabs(v1.v.position.y - v0.v.position.y) < 0.001);
        assert(v1.v.position.x >= v0.v.position.x);

        // calulcate Inverse slope in screen space
        float m0 = (v2.v.position.x - v0.v.position.x) / (v2.v.position.y - v0.v.position.y);
        float m1 = (v2.v.position.x - v1.v.position.x) / (v2.v.position.y - v1.v.position.y);

        // calculate start and end scanlines
        int yStart = (int)ceil(v0.v.position.y - 0.5f);
        const int yEnd = (int)ceil(v2.v.position.y - 0.5f); // the scanline AFTER the last line drawn

        const float unit0 = 1.f / (v2.v.position.y - v0.v.position.y), unit1 = 1.f / (v2.v.position.y - v1.v.position.y);
        float u0 = 0, u1 = 0;

        Vertex2 diff0 = v2 - v0, diff1 = v2 - v1, diff2;
        Vertex2 vx0, vx1, vx2;
        for (int y = yStart; y < yEnd && y < fboCPU->ymax; ++y, u0 += unit0, u1 += unit1) {
            if (y <= fboCPU->ymin) {
                continue;
            }
            // assert(u0 <= 1 && u1 <= 1);
            // caluclate start and end points (x-coords)
            // add 0.5 to y value because we're calculating based on pixel CENTERS
            const float px0 = m0 * (float(y) + 0.5f - v0.v.position.y) + v0.v.position.x;
            const float px1 = m1 * (float(y) + 0.5f - v1.v.position.y) + v1.v.position.x;

            vx0 = v0 + diff0 * u0;
            vx1 = v1 + diff1 * u1;

            // calculate start and end pixels
            int xStart = (int)ceil(px0 - 0.5f);
            const int xEnd = (int)ceil(px1 - 0.5f); // the pixel AFTER the last pixel drawn

            const float unit2 = 1.f / (xEnd - xStart);
            float u2 = 0;
            diff2 = vx1 - vx0;

            for (int x = xStart; x < xEnd && x < fboCPU->xmax; x++, u2 += unit2) {
                if (x <= fboCPU->xmin) {
                    continue;
                }
                float z = 1 / (vx0.v.position.w + (diff2.v.position.w) * u2);
#ifdef MULTITHREADED
                const std::lock_guard<std::mutex> lock(fboCPUMutex);
#endif
                auto gotz = getpixelZ_adjusted(x, y);
                if (gotz > z) {
                    vx2 = vx0 + diff2 * u2;
                    vx2 /= vx2.v.position.w;
                    putpixel_adjusted(x, y, z, getcolor(vx2, mesh));
                }
            }
        }
    }

    /**
 * @brief rasterize TopFlatTriangle: v0 is bottom unique and v2.x > v1.x
 */
    void fillTopFlatTriangle(const Vertex2 &v0, const Vertex2 &v1, const Vertex2 &v2, Mesh *mesh) {
        assert(fabs(v2.v.position.y - v1.v.position.y) < 0.01);
        assert(v2.v.position.x >= v1.v.position.x);
        // calulcate slopes in screen space
        float m0 = (v1.v.position.x - v0.v.position.x) / (v1.v.position.y - v0.v.position.y);
        float m1 = (v2.v.position.x - v0.v.position.x) / (v2.v.position.y - v0.v.position.y);

        // calculate start and end scanlines
        int yStart = (int)ceil(v0.v.position.y - 0.5f);
        const int yEnd = (int)ceil(v2.v.position.y - 0.5f); // the scanline AFTER the last line drawn

        const float unit0 = 1.f / (v1.v.position.y - v0.v.position.y), unit1 = 1.f / (v2.v.position.y - v0.v.position.y);
        float u0 = 0, u1 = 0;

        Vertex2 diff0 = v1 - v0, diff1 = v2 - v0, diff2;
        Vertex2 vx0, vx1, vx2;

        for (int y = yStart; y < yEnd && y < fboCPU->ymax; y++, u0 += unit0, u1 += unit1) {
            if (y <= fboCPU->ymin) {
                continue;
            }
            // caluclate start and end points
            // add 0.5 to y value because we're calculating based on pixel CENTERS
            const float px0 = m0 * (float(y) + 0.5f - v0.v.position.y) + v0.v.position.x;
            const float px1 = m1 * (float(y) + 0.5f - v0.v.position.y) + v0.v.position.x;

            vx0 = v0 + diff0 * u0;
            vx1 = v0 + diff1 * u1;

            // calculate start and end pixels
            int xStart = (int)ceil(px0 - 0.5f);
            const int xEnd = (int)ceil(px1 - 0.5f); // the pixel AFTER the last pixel drawn

            const float unit2 = 1.f / (xEnd - xStart);
            float u2 = 0;
            diff2 = vx1 - vx0;
            for (int x = xStart; x < xEnd && x < fboCPU->xmax; x++, u2 += unit2) {
                if (x <= fboCPU->xmin) {
                    continue;
                }
                float z = 1 / (vx0.v.position.w + (diff2.v.position.w) * u2);
#ifdef MULTITHREADED
                const std::lock_guard<std::mutex> lock(fboCPUMutex);
#endif
                const auto gotz = getpixelZ_adjusted(x, y);
                if (gotz > z) {
                    vx2 = vx0 + diff2 * u2;
                    vx2 /= vx2.v.position.w;
                    putpixel_adjusted(x, y, z, getcolor(vx2, mesh));
                }
            }
        }
    }

    /**
 * @brief helper function to rasterize triangles in vector of triangles.
 */
    void rasterizeTriangles() {

#ifdef MULTITHREADED
        pool.parallelize_loop(0, triangles.size(), [&](const unsigned int start, const unsigned int end) {
            for (unsigned int i = start; i < end; i++) {
                auto &tris = triangles[i];
                auto &v0 = std::get<0>(tris)[0];
                auto &v1 = std::get<0>(tris)[1];
                auto &v2 = std::get<0>(tris)[2];
                const auto &mesh = std::get<1>(tris);

                //sorting vertices by y
                if (v1.v.position.y < v0.v.position.y) {
                    std::swap(v0, v1);
                }
                if (v2.v.position.y < v1.v.position.y) {
                    std::swap(v1, v2);
                }
                if (v1.v.position.y < v0.v.position.y) {
                    std::swap(v0, v1);
                }

                if (v0.v.position.y == v1.v.position.y) // natural flat bottom
                {
                    // sorting bottom vertices by x
                    if (v1.v.position.x < v0.v.position.x) {
                        std::swap(v0, v1);
                    }
                    fillBottomFlatTriangle(v0, v1, v2, mesh);
                } else if (v1.v.position.y == v2.v.position.y) // natural flat top
                {
                    // sorting top vertices by x
                    if (v2.v.position.x < v1.v.position.x) {
                        std::swap(v1, v2);
                    }
                    fillTopFlatTriangle(v0, v1, v2, mesh);
                } else // general triangle
                {
                    assert(v2.v.position.y > v1.v.position.y && v1.v.position.y > v0.v.position.y);
                    // find splitting vertex
                    const float alphaSplit = (v1.v.position.y - v0.v.position.y) / (v2.v.position.y - v0.v.position.y);
                    const auto vi = v0 + (v2 - v0) * alphaSplit;

                    if (v1.v.position.x < vi.v.position.x) // major left
                    {
                        fillTopFlatTriangle(v0, v1, vi, mesh);
                        fillBottomFlatTriangle(v1, vi, v2, mesh);
                    } else // major right
                    {
                        fillTopFlatTriangle(v0, vi, v1, mesh);
                        fillBottomFlatTriangle(vi, v1, v2, mesh);
                    }
                }
            }
        });

#else
        int i = 0;
        for (auto &tris : triangles) {
            // auto tris = triangles[0];
            auto &v0 = std::get<0>(tris)[0];
            auto &v1 = std::get<0>(tris)[1];
            auto &v2 = std::get<0>(tris)[2];
            const auto &mesh = std::get<1>(tris);

            //sorting vertices by y
            if (v1.v.position.y < v0.v.position.y) {
                std::swap(v0, v1);
            }
            if (v2.v.position.y < v1.v.position.y) {
                std::swap(v1, v2);
            }
            if (v1.v.position.y < v0.v.position.y) {
                std::swap(v0, v1);
            }

            if (v0.v.position.y == v1.v.position.y) // natural flat bottom
            {
                // sorting bottom vertices by x
                if (v1.v.position.x < v0.v.position.x) {
                    std::swap(v0, v1);
                }
                fillBottomFlatTriangle(v0, v1, v2, mesh);
            } else if (v1.v.position.y == v2.v.position.y) // natural flat top
            {
                // sorting top vertices by x
                if (v2.v.position.x < v1.v.position.x) {
                    std::swap(v1, v2);
                }
                fillTopFlatTriangle(v0, v1, v2, mesh);
            } else // general triangle
            {
                assert(v2.v.position.y > v1.v.position.y && v1.v.position.y > v0.v.position.y);
                // find splitting vertex
                const float alphaSplit = (v1.v.position.y - v0.v.position.y) / (v2.v.position.y - v0.v.position.y);
                const auto vi = v0 + (v2 - v0) * alphaSplit;

                if (v1.v.position.x < vi.v.position.x) // major left
                {
                    fillTopFlatTriangle(v0, v1, vi, mesh);
                    fillBottomFlatTriangle(v1, vi, v2, mesh);
                } else // major right
                {
                    fillTopFlatTriangle(v0, vi, v1, mesh);
                    fillBottomFlatTriangle(vi, v1, v2, mesh);
                }
            }
        }
#endif
    }

    /**
 * @brief fill intensity information for each vertex needed for gouraud shading
 */
#ifndef PHONG_SHADING
    void fillExtraInformationForGoraudShading(Vertex &v, EXTRA_VERTEX_INFO &vertPos) {
        auto viewDir = (cam->eye - vertPos.x).normalize();
        for (auto &light : pointLights) {
            float dist = vec3::dist(vertPos.x, light.getpos());
            float int_by_at = light.intensity / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
            const vec3 lightDir = vec3::normalize(light.getpos() - vertPos.x);
            vertPos = {{0}, {0}, {0}};
            if (int_by_at > 0.01) {
                const float diff = max(vec3::dot(v.normal, lightDir), 0.0);
                const vec3 halfwayDir = vec3::normalize(lightDir + viewDir);
                const float spec = std::pow(max(vec3::dot(v.normal, halfwayDir), 0.0), currentMesh->material.shininess);
                vertPos.x += light.get_ambient_color().getcolorVec3() * currentMesh->material.AmbientStrength * int_by_at;
                vertPos.y += light.get_diffuse_color().getcolorVec3() * currentMesh->material.DiffuseStrength * diff * int_by_at;
                vertPos.z += light.get_diffuse_color().getcolorVec3() * currentMesh->material.SpecularStrength * spec * int_by_at;
            }
        }
    }
#endif
};
