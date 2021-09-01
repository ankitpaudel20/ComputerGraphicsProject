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
            clearedz[x] = -std::numeric_limits<float>::max();
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
        if (wireframe) {
            for (auto &tris : triangles) {
                draw_bresenham_adjusted(roundfloat(std::get<0>(tris)[0].v.position.x), roundfloat(std::get<0>(tris)[0].v.position.y), roundfloat(std::get<0>(tris)[1].v.position.x), roundfloat(std::get<0>(tris)[1].v.position.y), color(255));
                draw_bresenham_adjusted(roundfloat(std::get<0>(tris)[1].v.position.x), roundfloat(std::get<0>(tris)[1].v.position.y), roundfloat(std::get<0>(tris)[2].v.position.x), roundfloat(std::get<0>(tris)[2].v.position.y), color(255));
                draw_bresenham_adjusted(roundfloat(std::get<0>(tris)[2].v.position.x), roundfloat(std::get<0>(tris)[2].v.position.y), roundfloat(std::get<0>(tris)[0].v.position.x), roundfloat(std::get<0>(tris)[0].v.position.y), color(255));
            }
        } else {
            rasterizeTriangles();
            // for (auto &tris : triangles) {
            //     draw_bresenham_adjusted(roundfloat(std::get<0>(tris)[0].v.position.x), roundfloat(std::get<0>(tris)[0].v.position.y), roundfloat(std::get<0>(tris)[1].v.position.x), roundfloat(std::get<0>(tris)[1].v.position.y), color(255));
            //     draw_bresenham_adjusted(roundfloat(std::get<0>(tris)[1].v.position.x), roundfloat(std::get<0>(tris)[1].v.position.y), roundfloat(std::get<0>(tris)[2].v.position.x), roundfloat(std::get<0>(tris)[2].v.position.y), color(255));
            //     draw_bresenham_adjusted(roundfloat(std::get<0>(tris)[2].v.position.x), roundfloat(std::get<0>(tris)[2].v.position.y), roundfloat(std::get<0>(tris)[0].v.position.x), roundfloat(std::get<0>(tris)[0].v.position.y), color(255));
            // }
        }
        uint8_t *imageData = (uint8_t *)&fboCPU->colorlayer[0].col;
        glUseProgram(shader);
        GLcall(glBindTexture(GL_TEXTURE_2D, tex));
        GLcall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fboCPU->x_size, fboCPU->y_size, GL_RGBA, GL_UNSIGNED_BYTE, imageData));
        glBindVertexArray(vao);
        GLcall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
    }

    /**
 * @brief set of triangles to be drawn to screen
 */
    std::vector<std::tuple<std::array<Vertex2, 3>, Mesh *>> triangles;

    /**
 * @brief nearplane of camera for clipping and culling
 */
    float nearPlane = 1.0f;

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
    color ambientLightColor = color(255);
    dirLight dirlight;
    std::vector<pointLight> pointLights;

    /**
 * @brief camera
 */
    camera *cam = nullptr;

    bool wireframe = false;

    float lightRenderDistance = 0.05;

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
        // col = color(vec3(v.v.texCoord.x, v.v.texCoord.y, 1));
        // return col;
        color result(0, 0, 0, 255);
#ifdef PHONG_SHADING
        const auto viewDir = (cam->eye - v.extraInfoAboutVertex).normalize();
        for (auto &light : pointLights) {
            const float test = vec3::dist(cam->eye, light.getpos());
            // if (test > lightRenderDistance) {
            //     continue;
            // }
            float dist = vec3::dist(v.extraInfoAboutVertex, light.getpos());
            float int_by_at = light.intensity / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
            if (int_by_at > 0.01) {
                result += CalcPointLight(light, v.v.normal, v.extraInfoAboutVertex, viewDir, col, int_by_at, &mesh->material);
            }
        }
        result += CalcDirLight(v.v.normal, viewDir, col, &mesh->material);
#else
        // result += col * (v.extraInfoAboutVertex.x);
        result += col * (v.extraInfoAboutVertex.y);
        result += color(v.extraInfoAboutVertex.z);
#endif
        result += col * (ambientLightColor * ambientLightIntensity);

        return result;
    }

    bool printed = false;
    /**
 * @brief fill triangles array by doing required clipping and culling
 */
    void makeRequiredTriangles(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const mat4f &modelmat) {
        const auto view = trans::lookAt(cam->eye, cam->eye + cam->getViewDir(), cam->getUp());
        const auto per = trans::persp(fboCPU->x_size, fboCPU->y_size, cam->FOV);
        // #ifdef MULTITHREADED
        //         pool.parallelize_loop(0, indices.size() / 3, [&](const unsigned int start, const unsigned int end) {
        //             for (unsigned int i = start; i < end; i++) {
        //                 std::array<Vertex, 3> points = {vertices[indices[i * 3]], vertices[indices[i * 3 + 1]], vertices[indices[i * 3 + 2]]};
        //                 makeRequiredTrianglesHelper(points, modelmat, view, per);
        //             }
        //         });
        // #else
        // #endif
        for (size_t i = 0; i < indices.size() / 3; i++) {
            std::array<Vertex, 3> points = {vertices[indices[i * 3]], vertices[indices[i * 3 + 1]], vertices[indices[i * 3 + 2]]};
            makeRequiredTrianglesHelper(points, modelmat, view, per);
        }

        // printf("triangle_count: %d\n", triangles.size());
        // printf("\033[F");
    }

    void setTime(float time) {
        assert(time >= 0.f && time < 24.f);
        dirlight.intensity = 0.7666921f * exp(-pow((time - 12.4125f) / (3.047894f), 2.f) / 2.f);
        ambientLightIntensity = dirlight.intensity * 0.5;
        const uint8_t min = floor(time), max = ceil(time);
        float alpha;
        if (min == max)
            alpha = 0;
        else
            alpha = (max - time) / (max - min);

        float green = greenTimeMap[min] + alpha * (greenTimeMap[max] - greenTimeMap[min]);
        float blue = blueTimeMap[min] + alpha * (blueTimeMap[max] - blueTimeMap[min]);
        ambientLightColor = color(vec3(1, green, blue));
        const float t = M_PI * sin(M_PI * time / 24);
        dirlight.direction = vec3(sin(t), -cos(t), 0);
        dirlight.direction.normalize();

        printf("time: %f\n", time);
        printf("ambientlight color: (%d,%d,%d)\n", ambientLightColor.r(), ambientLightColor.g(), ambientLightColor.b());
        printf("dirlight intensity: %f\n", dirlight.intensity);
        printVec(dirlight.direction);

        printf("\033[F");
        printf("\033[F");
        printf("\033[F");
        printf("\033[F");
    }

  private:
    std::unordered_map<uint8_t, float>
        greenTimeMap = {{0, 0}, {1, 0.1}, {2, 0.3}, {3, 0.3}, {4, 0.35}, {5, 0.4}, {6, 0.5}, {7, 0.55}, {8, 0.6}, {9, 1}, {10, 1}, {11, 1}, {12, 1}, {13, 1}, {14, 1}, {15, 1}, {16, 1}, {17, 0.8}, {18, 0.5}, {19, 0.4}, {20, 0.3}, {21, 0.25}, {22, 0.1}, {23, 0.06}, {24, 0}},
        blueTimeMap = {
            {0, 0},
            {1, 0},
            {2, 0},
            {3, 0.05},
            {4, 0.06},
            {5, 0.1},
            {6, 0.15},
            {7, 0.25},
            {8, 0.35},
            {9, 0.75},
            {10, 0.8},
            {11, 1},
            {12, 1},
            {13, 1},
            {14, 1},
            {15, 1},
            {16, 0.8},
            {17, 0.6},
            {18, 0.4},
            {19, 0.1},
            {20, 0.01},
            {21, 0},
            {22, 0},
            {23, 0},
            {24, 0}

    };

    static inline float max(const float &first, const float &second) {
        return first > second ? first : second;
    }

    static inline vec3 reflect(const vec3 &I, const vec3 &N) {
        return I - N * 2.0 * vec3::dot(N, I);
    }

    /**
 * @brief function to calculate effect of directional light in a fragment with color col
 */
    color CalcDirLight(const vec3 &normal, const vec3 &viewdir, const color &col, const Material *material) const {
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

    void makeRequiredTrianglesHelper(std::array<Vertex, 3> &points, const mat4f &modelmat, const mat4f &view, const mat4f &per) {
        std::array<bool, 3> clip{false, false, false};
        std::array<Vertex2, 3> t;
// Vertex temp;
#ifdef PHONG_SHADING
        std::array<EXTRA_VERTEX_INFO, 3> extraInfoAboutVertex{modelmat * points[0].position, modelmat * points[1].position, modelmat * points[2].position};
#else
        std::array<EXTRA_VERTEX_INFO, 3> extraInfoAboutVertex;
        extraInfoAboutVertex[0].x = modelmat * points[0].position;
        extraInfoAboutVertex[1].x = modelmat * points[1].position;
        extraInfoAboutVertex[2].x = modelmat * points[2].position;
#endif
        std::array<vec4, 3> modelviewTransformed{view * (modelmat * points[0].position), view * (modelmat * points[1].position), view * (modelmat * points[2].position)};

        if (modelviewTransformed[0].z > -nearPlane) {
            clip[0] = true;
        }

        if (modelviewTransformed[1].z > -nearPlane) {
            clip[1] = true;
        }

        if (modelviewTransformed[2].z > -nearPlane) {
            clip[2] = true;
        }

        points[0].position = modelviewTransformed[0];
        points[0].normal = modelmat * points[0].normal;
        points[1].position = modelviewTransformed[1];
        points[1].normal = modelmat * points[1].normal;
        points[2].position = modelviewTransformed[2];
        points[2].normal = modelmat * points[2].normal;

        vec4 centroid = (points[0].position + points[1].position + points[2].position) / 3;

        if (cullBackface && vec3::dot(view * points[0].normal, (vec3)centroid) > 0) {
            return;
        }

#ifndef PHONG_SHADING
        fillExtraInformationForGoraudShading(points[0], extraInfoAboutVertex[0]);
        fillExtraInformationForGoraudShading(points[1], extraInfoAboutVertex[1]);
        fillExtraInformationForGoraudShading(points[2], extraInfoAboutVertex[2]);
#endif

        if ((clip[0] && clip[1] && clip[2])) {
            return;
        } else if (clip[0]) {
            if (clip[1]) {
                clip2helper(per, extraInfoAboutVertex, modelviewTransformed, points.data(), 0, 1, 2, t);
                triangles.emplace_back(std::make_tuple(t, currentMesh));
                return;
            } else if (clip[2]) {
                clip2helper(per, extraInfoAboutVertex, modelviewTransformed, points.data(), 0, 2, 1, t);
                triangles.emplace_back(std::make_tuple(t, currentMesh));
                return;
            } else {
                clip1helper(per, extraInfoAboutVertex, modelviewTransformed, points.data(), 0, t, triangles);
                return;
            }
        } else if (clip[1]) {
            if (clip[2]) {
                clip2helper(per, extraInfoAboutVertex, modelviewTransformed, points.data(), 1, 2, 0, t);
                triangles.emplace_back(std::make_tuple(t, currentMesh));
                return;
            } else if (clip[0]) {
                clip2helper(per, extraInfoAboutVertex, modelviewTransformed, points.data(), 1, 0, 2, t);
                triangles.emplace_back(std::make_tuple(t, currentMesh));
                return;
            } else {
                clip1helper(per, extraInfoAboutVertex, modelviewTransformed, points.data(), 1, t, triangles);
                return;
            }
        } else if (clip[2]) {
            if (clip[0]) {
                clip2helper(per, extraInfoAboutVertex, modelviewTransformed, points.data(), 2, 0, 1, t);
                triangles.emplace_back(std::make_tuple(t, currentMesh));
                return;
            } else if (clip[1]) {
                clip2helper(per, extraInfoAboutVertex, modelviewTransformed, points.data(), 2, 1, 0, t);
                triangles.emplace_back(std::make_tuple(t, currentMesh));
                return;
            } else {
                clip1helper(per, extraInfoAboutVertex, modelviewTransformed, points.data(), 2, t, triangles);
                return;
            }
        }

        t[0] = Vertex2(Vertex::perspectiveMul(points[0], per), extraInfoAboutVertex[0]);
        t[1] = Vertex2(Vertex::perspectiveMul(points[1], per), extraInfoAboutVertex[1]);
        t[2] = Vertex2(Vertex::perspectiveMul(points[2], per), extraInfoAboutVertex[2]);
        t[0].extraInfoAboutVertex *= t[0].v.position.z;
        t[1].extraInfoAboutVertex *= t[1].v.position.z;
        t[2].extraInfoAboutVertex *= t[2].v.position.z;

        if ((t[0].v.position.x > fboCPU->xmax || t[0].v.position.x < fboCPU->xmin) && (t[0].v.position.y > fboCPU->ymax || t[0].v.position.y < fboCPU->ymin) &&
            (t[1].v.position.x > fboCPU->xmax || t[1].v.position.x < fboCPU->xmin) && (t[1].v.position.y > fboCPU->ymax || t[1].v.position.y < fboCPU->ymin) &&
            (t[2].v.position.x > fboCPU->xmax || t[2].v.position.x < fboCPU->xmin) && (t[2].v.position.y > fboCPU->ymax || t[2].v.position.y < fboCPU->ymin))
            return;

        triangles.emplace_back(std::make_tuple(t, currentMesh));
    }

    /**
 * @brief putpixel assuming middle of screen to be the origin
 */
    void
    putpixel_adjusted(int x, int y, float z, const color &col = color(255)) {
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
        return test ? fboCPU->z[((size_t)x + fboCPU->xmax) + ((size_t)y + fboCPU->ymax) * fboCPU->x_size] : std::numeric_limits<float>::max();
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

            if (!changed) {
                if (x < fboCPU->xmax && x > fboCPU->xmin && y < fboCPU->ymax && y > fboCPU->ymin)
                    putpixel_adjusted(x, y, 1, col);
            } else {
                if (y < fboCPU->xmax && y > fboCPU->xmin && x < fboCPU->ymax && x > fboCPU->ymin)
                    putpixel_adjusted(y, x, 1, col);
            }

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

    inline void clip1(const std::array<vec4, 3> &tris, unsigned char which, float &u1, float &u2) const {
        u1 = -(nearPlane + tris[which].z) / (tris[(which + 1) % 3].z - tris[which].z);
        u2 = -(nearPlane + tris[which].z) / (tris[(which + 2) % 3].z - tris[which].z);
    }

    inline void clip2(const std::array<vec4, 3> &tris, unsigned char idx1, unsigned char idx2, unsigned char rem, float &u1, float &u2) const {
        u1 = -(nearPlane + tris[idx1].z) / (tris[rem].z - tris[idx1].z);
        u2 = -(nearPlane + tris[idx2].z) / (tris[rem].z - tris[idx2].z);
    }

    /**
 * @brief clips and perspective transforms input triangle vertices if two points are out of clip space
 */
    inline void clip2helper(const mat4f &per, const std::array<EXTRA_VERTEX_INFO, 3> &extraInfoAboutVertex, const std::array<vec4, 3> &modelviewTransformed, Vertex *points, unsigned char idx1, unsigned char idx2, unsigned char rem, std::array<Vertex2, 3> &t) {
        float u1, u2;
        clip2(modelviewTransformed, idx1, idx2, rem, u1, u2);

        t[idx1] = Vertex2(Vertex::perspectiveMul(points[idx1] + (points[rem] - points[idx1]) * u1, per), extraInfoAboutVertex[idx1] + (extraInfoAboutVertex[rem] - extraInfoAboutVertex[idx1]) * u1);
        t[idx2] = Vertex2(Vertex::perspectiveMul(points[idx2] + (points[rem] - points[idx2]) * u2, per), extraInfoAboutVertex[idx2] + (extraInfoAboutVertex[rem] - extraInfoAboutVertex[idx2]) * u2);
        t[rem] = Vertex2(Vertex(modelviewTransformed[rem], points[rem].normal, points[rem].texCoord).perspectiveMul(per), extraInfoAboutVertex[rem]);

        t[idx1].extraInfoAboutVertex *= t[idx1].v.position.z;
        t[idx2].extraInfoAboutVertex *= t[idx2].v.position.z;
        t[rem].extraInfoAboutVertex *= t[rem].v.position.z;
    }

    /**
 * @brief clips and perspective transforms input triangle vertices if one point is out of the clip space
 */
    inline void clip1helper(const mat4f &per, const std::array<EXTRA_VERTEX_INFO, 3> &extraInfoAboutVertex, const std::array<vec4, 3> &modelviewTransformed, Vertex *points, unsigned char idx1, std::array<Vertex2, 3> &t, std::vector<std::tuple<std::array<Vertex2, 3>, Mesh *>> &triangles) {
        float u1, u2;
        unsigned char idx2 = (idx1 + 1) % 3, idx3 = (idx1 + 2) % 3;
        clip1(modelviewTransformed, idx1, u1, u2);

        t[idx1] = Vertex2(Vertex::perspectiveMul(points[idx1] + (points[idx2] - points[idx1]) * u1, per), extraInfoAboutVertex[idx1] + (extraInfoAboutVertex[idx2] - extraInfoAboutVertex[idx1]) * u1);
        t[idx2] = Vertex2(Vertex(modelviewTransformed[idx2], points[idx2].normal, points[idx2].texCoord).perspectiveMul(per), extraInfoAboutVertex[idx2]);
        t[idx3] = Vertex2(Vertex(modelviewTransformed[idx3], points[idx3].normal, points[idx3].texCoord).perspectiveMul(per), extraInfoAboutVertex[idx3]);
        t[idx1].extraInfoAboutVertex *= t[idx1].v.position.z;
        t[idx2].extraInfoAboutVertex *= t[idx2].v.position.z;
        t[idx3].extraInfoAboutVertex *= t[idx3].v.position.z;
        const std::lock_guard<std::mutex> lock(fboCPUMutex);
        triangles.emplace_back(std::make_tuple(t, currentMesh));
        t[idx2] = t[idx3];
        t[idx3] = Vertex2(Vertex::perspectiveMul(points[idx1] + (points[idx3] - points[idx1]) * u2, per), extraInfoAboutVertex[idx1] + (extraInfoAboutVertex[idx3] - extraInfoAboutVertex[idx1]) * u2);
        t[idx3].extraInfoAboutVertex *= t[idx3].v.position.z;
        triangles.emplace_back(std::make_tuple(t, currentMesh));
    }

    /**
 * @brief rasterize BottomFlatTriangle: v2 is top unique and v1.x > v0.x
 */
    void fillBottomFlatTriangle(const Vertex2 &v0, const Vertex2 &v1, const Vertex2 &v2, Mesh *mesh) {
        assert(fabs(v1.v.position.y - v0.v.position.y) < 0.01);
        assert(v1.v.position.x >= v0.v.position.x);
        // calulcate slopes in screen space
        const double m0 = (v2.v.position.x - v0.v.position.x) / (v2.v.position.y - v0.v.position.y);
        const double m1 = (v2.v.position.x - v1.v.position.x) / (v2.v.position.y - v1.v.position.y);

        // calculate start and end scanlines
        int yStart = (int)ceil(v0.v.position.y - 0.5f);
        const int yEnd = (int)ceil(v2.v.position.y - 0.5f); // the scanline AFTER the last line drawn

        const double unit0 = 1.f / (v2.v.position.y - v0.v.position.y);
        double u0 = 0;

        Vertex2 diff0 = v2 - v0, diff1 = v2 - v1, diff2;
        Vertex2 vx0, vx1, vx2;

        if (yStart <= fboCPU->ymin) {
            u0 = unit0 * (fboCPU->ymin - yStart + 1);
            yStart = fboCPU->ymin + 1;
        }
        for (int y = yStart; y < yEnd && y < fboCPU->ymax; ++y, u0 += unit0) {
            const double px0 = m0 * (double(y) + 0.5f - v0.v.position.y) + v0.v.position.x;
            const double px1 = m1 * (double(y) + 0.5f - v1.v.position.y) + v1.v.position.x;

            vx0 = v0 + diff0 * u0;
            vx1 = v1 + diff1 * u0;

            // calculate start and end pixels
            int xStart = (int)ceil(px0 - 0.5f);
            const int xEnd = (int)ceil(px1 - 0.5f); // the pixel AFTER the last pixel drawn

            const double unit2 = 1.f / (px1 - px0);
            double u2 = 0;
            diff2 = vx1 - vx0;

            if (xStart <= fboCPU->xmin) {
                u2 = unit2 * (fboCPU->xmin - xStart + 1);
                xStart = fboCPU->xmin + 1;
            }
            for (int x = xStart; x < xEnd && x < fboCPU->xmax; x++, u2 += unit2) {
                const double z = 1 / (vx0.v.position.z + (diff2.v.position.z) * u2);
#ifdef MULTITHREADED
                const std::lock_guard<std::mutex> lock(fboCPUMutex);
#endif
                auto gotz = getpixelZ_adjusted(x, y);
                if (gotz < z) {
                    vx2 = vx0 + diff2 * u2;
                    vx2.v.texCoord /= vx2.v.position.z;
                    vx2.v.normal /= vx2.v.position.z;
                    vx2.extraInfoAboutVertex /= vx2.v.position.z;
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
        const double m0 = (v1.v.position.x - v0.v.position.x) / (v1.v.position.y - v0.v.position.y);
        const double m1 = (v2.v.position.x - v0.v.position.x) / (v2.v.position.y - v0.v.position.y);

        // calculate start and end scanlines
        const int yStart = (int)ceil(v0.v.position.y - 0.5);
        const int yEnd = (int)ceil(v2.v.position.y - 0.5); // the scanline AFTER the last line drawn

        const double unit0 = 1.0 / (v1.v.position.y - v0.v.position.y), unit1 = 1.0 / (v2.v.position.y - v0.v.position.y);
        double u0 = 0, u1 = 0;

        Vertex2 diff0 = v1 - v0, diff1 = v2 - v0, diff2;
        Vertex2 vx0, vx1, vx2;

        for (int y = yStart; y < yEnd && y < fboCPU->ymax; y++, u0 += unit0, u1 += unit1) {
            if (y <= fboCPU->ymin) {
                continue;
            }
            const double px0 = m0 * (double(y) + 0.5f - v0.v.position.y) + v0.v.position.x;
            const double px1 = m1 * (double(y) + 0.5f - v0.v.position.y) + v0.v.position.x;

            vx0 = v0 + diff0 * u0;
            vx1 = v0 + diff1 * u1;

            // calculate start and end pixels
            const int xStart = (int)ceil(px0 - 0.5f);
            const int xEnd = (int)ceil(px1 - 0.5f); // the pixel AFTER the last pixel drawn

            const double unit2 = 1.0 / (px1 - px0);
            double u2 = 0;
            diff2 = vx1 - vx0;

            for (int x = xStart; x < xEnd && x < fboCPU->xmax; x++, u2 += unit2) {
                if (x <= fboCPU->xmin) {
                    continue;
                }
                double z = 1 / (vx0.v.position.z + (diff2.v.position.z) * u2);
#ifdef MULTITHREADED
                const std::lock_guard<std::mutex> lock(fboCPUMutex);
#endif
                const auto gotz = getpixelZ_adjusted(x, y);
                if (gotz < z) {
                    vx2 = vx0 + diff2 * u2;
                    vx2.v.texCoord /= vx2.v.position.z;
                    vx2.v.normal /= vx2.v.position.z;
                    vx2.extraInfoAboutVertex /= vx2.v.position.z;
                    putpixel_adjusted(x, y, z, getcolor(vx2, mesh));
                }
            }
        }
    }

    void calculateBarycentricCoordinates(const vec2 &p, const vec4 &v1, const vec4 &v2, const vec4 &v3, std::array<float, 3> &values) {
        const float area = (v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y);
        values[0] = fabs(((v2.y - v3.y) * (p.x - v3.x) + (v3.x - v2.x) * (p.y - v3.y)) / area);
        values[1] = fabs(((v3.y - v1.y) * (p.x - v3.x) + (v1.x - v3.x) * (p.y - v3.y)) / area);
        values[2] = 1.f - values[0] - values[1];

        // if (fabs(values[0]) > 1 && fabs(values[1]) > 1 && fabs(values[2]) > 1) {
        //     printVec(vec2(v1));
        //     printVec(vec2(v2));
        //     printVec(vec2(v3));
        //     printVec(p);
        //     assert(false);
        // }

        // float dist[] = {vec2::dist(p, vec2(v1)), vec2::dist(p, vec2(v2)), vec2::dist(p, vec2(v3))};
        // if (dist[0] > dist[1] && dist[0] > dist[2]) {
        //     if (fabs(values[0]) > fabs(values[1]) && fabs(values[0]) > fabs(values[2])) {
        //         printVec(vec2(v1));
        //         printVec(vec2(v2));
        //         printVec(vec2(v3));
        //         printVec(p);
        //         assert(false);
        //     }
        // } else if (dist[1] > dist[0] && dist[1] > dist[2]) {
        //     if (fabs(values[1]) > fabs(values[0]) && fabs(values[1]) > fabs(values[2])) {
        //         printVec(vec2(v1));
        //         printVec(vec2(v2));
        //         printVec(vec2(v3));
        //         printVec(p);
        //         assert(false);
        //     }
        // } else {
        //     if (fabs(values[2]) > fabs(values[1]) && fabs(values[2]) > fabs(values[0])) {
        //         printVec(vec2(v1));
        //         printVec(vec2(v2));
        //         printVec(vec2(v3));
        //         printVec(p);
        //         assert(false);
        //     }
        // }
    }

    /**
 * @brief rasterize BottomFlatTriangle: v2 is top unique and v1.x > v0.x
 */

    void fillBottomFlatTriangleBary(const Vertex2 &v0, const Vertex2 &v1, const Vertex2 &v2, Mesh *mesh) {
        assert(fabs(v1.v.position.y - v0.v.position.y) < 0.01);
        assert(v1.v.position.x >= v0.v.position.x);

        const double m0 = (double)(v2.v.position.x - v0.v.position.x) / (v2.v.position.y - v0.v.position.y);
        const double m1 = (double)(v2.v.position.x - v1.v.position.x) / (v2.v.position.y - v1.v.position.y);

        const int yStart = (int)ceil(v0.v.position.y - 0.5f);
        const int yEnd = (int)ceil(v2.v.position.y - 0.5f); // the scanline AFTER the last line drawn

        const double unit0 = (v2.v.position.y - v0.v.position.y) / (yEnd - yStart);
        double u0 = 0;

        Vertex2 vx2;
        std::array<float, 3> coefficients;

        for (int y = yStart; y < yEnd && y < fboCPU->ymax; ++y, u0 += unit0) {
            if (y <= fboCPU->ymin) {
                continue;
            }
            const double px0 = m0 * (double(y) + 0.5 - v0.v.position.y) + v0.v.position.x;
            const double px1 = m1 * (double(y) + 0.5 - v1.v.position.y) + v1.v.position.x;

            const int xStart = (int)ceil(px0 - 0.5);
            const int xEnd = (int)ceil(px1 - 0.5);

            const double unit2 = (px1 - px0) / (xEnd - xStart);
            double u2 = 0;

            for (int x = xStart; x < xEnd && x < fboCPU->xmax; x++, u2 += unit2) {
                if (x <= fboCPU->xmin) {
                    continue;
                }
                const vec2 pos(px0 + u2, v0.v.position.y + u0);
                calculateBarycentricCoordinates(pos, v0.v.position, v1.v.position, v2.v.position, coefficients);

                double z = 1 / (coefficients[0] * v0.v.position.z + coefficients[1] * v1.v.position.z + coefficients[2] * v2.v.position.z);
#ifdef MULTITHREADED
                const std::lock_guard<std::mutex> lock(fboCPUMutex);
#endif
                auto gotz = getpixelZ_adjusted(x, y);
                if (gotz < z) {
                    vx2 = v0 * coefficients[0] + v1 * coefficients[1] + v2 * coefficients[2];
                    vx2.v.texCoord /= vx2.v.position.z;
                    vx2.v.normal /= vx2.v.position.z;
                    vx2.extraInfoAboutVertex /= vx2.v.position.z;
                    putpixel_adjusted(x, y, z, getcolor(vx2, mesh));
                }
            }
        }
    }

    void fillTopFlatTriangleBary(const Vertex2 &v0, const Vertex2 &v1, const Vertex2 &v2, Mesh *mesh) {
        assert(fabs(v2.v.position.y - v1.v.position.y) < 0.01);
        assert(v2.v.position.x >= v1.v.position.x);
        // calulcate slopes in screen space
        const double m0 = (v1.v.position.x - v0.v.position.x) / (v1.v.position.y - v0.v.position.y);
        const double m1 = (v2.v.position.x - v0.v.position.x) / (v2.v.position.y - v0.v.position.y);

        // calculate start and end scanlines
        const int yStart = (int)ceil(v0.v.position.y - 0.5);
        const int yEnd = (int)ceil(v2.v.position.y - 0.5); // the scanline AFTER the last line drawn

        const double unit0 = (v2.v.position.y - v0.v.position.y) / (yEnd - yStart);
        double u0 = 0;

        Vertex2 vx2;
        std::array<float, 3> coefficients;

        for (int y = yStart; y < yEnd && y < fboCPU->ymax; ++y, u0 += unit0) {
            if (y <= fboCPU->ymin) {
                continue;
            }
            const double px0 = m0 * (double(y) + 0.5f - v0.v.position.y) + v0.v.position.x;
            const double px1 = m1 * (double(y) + 0.5f - v0.v.position.y) + v0.v.position.x;

            const int xStart = (int)ceil(px0 - 0.5);
            const int xEnd = (int)ceil(px1 - 0.5);

            const double unit2 = (px1 - px0) / (xEnd - xStart);
            double u2 = 0;

            for (int x = xStart; x < xEnd && x < fboCPU->xmax; x++, u2 += unit2) {
                if (x <= fboCPU->xmin) {
                    continue;
                }
                const vec2 pos(px0 + u2, v0.v.position.y + u0);
                calculateBarycentricCoordinates(pos, v0.v.position, v1.v.position, v2.v.position, coefficients);

                double z = 1 / (coefficients[0] * v0.v.position.z + coefficients[1] * v1.v.position.z + coefficients[2] * v2.v.position.z);
#ifdef MULTITHREADED
                const std::lock_guard<std::mutex> lock(fboCPUMutex);
#endif
                auto gotz = getpixelZ_adjusted(x, y);
                if (gotz < z) {
                    vx2 = v0 * coefficients[0] + v1 * coefficients[1] + v2 * coefficients[2];
                    vx2.v.texCoord /= vx2.v.position.z;
                    vx2.v.normal /= vx2.v.position.z;
                    vx2.extraInfoAboutVertex /= vx2.v.position.z;
                    putpixel_adjusted(x, y, z, getcolor(vx2, mesh));
                }
            }
        }
    }

    void rasterizeTrianglesHelper(std::tuple<std::array<Vertex2, 3>, Mesh *> &tris) {
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
            if (v1.v.position.x < v0.v.position.x) {
                std::swap(v0, v1);
            }
            fillBottomFlatTriangle(v0, v1, v2, mesh);
        } else if (v1.v.position.y == v2.v.position.y) // natural flat top
        {
            if (v2.v.position.x < v1.v.position.x) {
                std::swap(v1, v2);
            }
            fillTopFlatTriangle(v0, v1, v2, mesh);
        } else // general triangle
        {
            assert(v2.v.position.y > v1.v.position.y && v1.v.position.y > v0.v.position.y);
            // find splitting vertex
            const double alphaSplit = (v1.v.position.y - v0.v.position.y) / (v2.v.position.y - v0.v.position.y);
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
            // draw_bresenham_adjusted(roundfloat(vi.v.position.x), roundfloat(vi.v.position.y), roundfloat(v1.v.position.x), roundfloat(v1.v.position.y), color(0, 255, 0));
        }
    }

    /**
 * @brief helper function to rasterize triangles in vector of triangles.
 */
    void rasterizeTriangles() {
#ifdef MULTITHREADED
        pool.parallelize_loop(0, triangles.size(), [&](const unsigned int start, const unsigned int end) {
            for (unsigned int i = start; i < end; i++) {
                rasterizeTrianglesHelper(triangles[i]);
            }
        });

#else
        for (auto &tris : triangles) {
            rasterizeTrianglesHelper(tris);
        }
#endif
    }

    /**
 * @brief fill intensity information for each vertex needed for gouraud shading
 */
#ifndef PHONG_SHADING
    void fillExtraInformationForGoraudShading(Vertex &v, EXTRA_VERTEX_INFO &vertPos) const {
        const auto viewDir = (cam->eye - vertPos.x).normalize();
        vertPos = EXTRA_VERTEX_INFO(vec3(0), vec3(0), vec3(0));
        for (auto &light : pointLights) {
            float dist = vec3::dist(vertPos.x, light.getpos());
            float int_by_at = light.intensity / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
            const vec3 lightDir = vec3::normalize(light.getpos() - vertPos.x);
            if (int_by_at > 0.000001) {
                const float diff = max(vec3::dot(v.normal, lightDir), 0.0);
                const vec3 halfwayDir = vec3::normalize(lightDir + viewDir);
                const float spec = std::pow(max(vec3::dot(v.normal, halfwayDir), 0.0), currentMesh->material.shininess);
                vertPos.x += light.get_ambient_color().getcolorVec3() * currentMesh->material.AmbientStrength * light.intensity * int_by_at;
                vertPos.y += light.get_diffuse_color().getcolorVec3() * currentMesh->material.DiffuseStrength * light.intensity * diff * int_by_at;
                vertPos.z += light.get_diffuse_color().getcolorVec3() * currentMesh->material.SpecularStrength * light.intensity * spec * int_by_at;
            }
        }
    }
#endif
};