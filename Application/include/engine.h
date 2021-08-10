#pragma once
#include "core.h"
#include <functional>
#include <algorithm>
#include <limits>
#include "material.h"
#include "pointLight.h"

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.h"

inline int roundfloat(const float &in) {
    return in < 0 ? in - 0.5f : in + 0.5f;
}

inline int rounddouble(const double &in) {
    return in < 0 ? in - 0.5f : in + 0.5f;
}

struct framebuffer {
    bool *grid;
    float *z;
    color *colorlayer;
    bool *clearedgrid;
    float *clearedz;
    color *clearedcolorlayer;
    size_t x_size, y_size;
    int xmax, xmin, ymax, ymin;
    framebuffer(const size_t &x, const size_t &y) : x_size(x), y_size(y) {
        grid = new bool[x_size * y_size];
        colorlayer = new color[x_size * y_size];
        z = new float[x_size * y_size];

        clearedgrid = new bool[x_size * y_size];
        clearedcolorlayer = new color[x_size * y_size];
        clearedz = new float[x_size * y_size];
        for (uint32_t x = 0; x < x_size * y_size; ++x) {
            clearedgrid[x] = false;
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
        delete[] grid;
        delete[] colorlayer;
        delete[] z;
    }

    void clear() {
        memcpy(grid, clearedgrid, x_size * y_size * sizeof(bool));
        memcpy(colorlayer, clearedcolorlayer, x_size * y_size * sizeof(color));
        memcpy(z, clearedz, x_size * y_size * sizeof(float));
    }
};

//Vertex + extra info required for shading
struct Vertex2 {
    Vertex v;
    //fragment Position for phong and INTENSITY for gouraud
    vec3 f_pos;
    Vertex2(const Vertex &vin, const vec3 &pos) : v(vin), f_pos(pos) {}
    Vertex2() {}
    Vertex2 operator*(const float &f) const {
        return Vertex2(v * f, f_pos * f);
    }
    Vertex2 operator+(const Vertex2 &f) const {
        return Vertex2(f.v + v, f.f_pos + f_pos);
    }
    Vertex2 operator-(const Vertex2 &f) const {
        return Vertex2(v - f.v, f_pos - f.f_pos);
    }
};

struct engine {

    bool written = false;
    framebuffer *fboCPU;

    uint32_t vao, vbo, ibo, tex, shader;

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

    //constructor and opengl draw surface initialization
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

    //destructor
    ~engine() {
        delete fboCPU;
        GLcall(glDeleteBuffers(1, &vbo));
        GLcall(glDeleteVertexArrays(1, &vao));
        GLcall(glDeleteTextures(1, &tex));
        GLcall(glDeleteProgram(shader));
    }

    //clear all drawing surface
    void clear() {
        triangles.clear();
        glClear(GL_COLOR_BUFFER_BIT);
        fboCPU->clear();
    }

    //send framebuffer from cpu to gpu as texture to be able to render using opengl
    void draw() {
        uint8_t *imageData = (uint8_t *)&fboCPU->colorlayer[0].col;
        glUseProgram(shader);
        GLcall(glBindTexture(GL_TEXTURE_2D, tex));
        GLcall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fboCPU->x_size, fboCPU->y_size, GL_RGBA, GL_UNSIGNED_BYTE, imageData));
        glBindVertexArray(vao);
        GLcall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
    }   

    //drawlines using given vertices and indices
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

    //draw linestrip fron given set of vertices, indices and color
    void drawLinestrip(const std::vector<vec2_T<int>> &points, const color &col = color(255), const std::vector<uint32_t> &indices = std::vector<uint32_t>()) {
        for (size_t i = 0; i < points.size(); i++) {
            draw_bresenham_adjusted(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y, col);
        }
    }

    //set of triangles to be drawn to screen
    std::vector<std::array<Vertex2, 3>> triangles;

    //nearplane of camera for clipping and culling
    float nearPlane = 0.0f;

    //switch for backface culling
    bool cullBackface = true;    

    //which material to use for further render calls
    Material *currentMaterial = nullptr;

    float ambientLightIntensity = 0.1f;
    dirLight dirlight;

    //vectors of pointLights.
    std::vector<pointLight> pointLights;

    camera *cam = nullptr;


    //essentially a fragment shader : inputs fragment position and information and outputs color value for the fragment
    color getcolor(const Vertex2 &v) {
        color col;
        if (currentMaterial->diffuse.w) {
            float intpart;
            int tx = roundfloat(std::modf(v.v.texCoord.x / v.v.position.z, &intpart) * (currentMaterial->diffuse.w - 1));
            int ty = roundfloat(std::modf(v.v.texCoord.y / v.v.position.z, &intpart) * (currentMaterial->diffuse.h - 1));
            auto ret = &currentMaterial->diffuse.m_data[(abs(tx) + currentMaterial->diffuse.w * abs(ty)) * currentMaterial->diffuse.m_bpp];
            col = color(*ret, *(ret + 1), *(ret + 2));
        } else
            col = currentMaterial->diffuseColor;

        color result;
#ifdef PHONG_SHADING
        auto viewDir = (cam->eye - v.f_pos).normalize();
        for (auto &light : pointLights) {
            float dist = vec3::dist(v.f_pos, light.getpos());
            float int_by_at = light.intensity / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
            if (int_by_at > 0.04) {
                result += CalcPointLight(light, v.v.normal, v.f_pos, viewDir, col, int_by_at);
            }
        }
        result += col * ambientLightIntensity;
#else
        for (auto &light : pointLights) {
            result += col * light.get_ambient_color() * v.f_pos.x;
            result += col * light.get_diffuse_color() * v.f_pos.y;
            result += light.get_diffuse_color() * v.f_pos.z;
            result += col * ambientLightIntensity;
        }
#endif

        return std::move(result);
    }    

    void drawTriangles(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const mat4f &modelmat) {
        triangles.clear();
        makeRequiredTriangles(vertices, indices, modelmat);
        for (auto &tris : triangles) {
            draw_bresenham_adjusted(roundfloat(tris[0].v.position.x), roundfloat(tris[0].v.position.y), roundfloat(tris[1].v.position.x), roundfloat(tris[1].v.position.y), color(0, 255, 0));
            draw_bresenham_adjusted(roundfloat(tris[1].v.position.x), roundfloat(tris[1].v.position.y), roundfloat(tris[2].v.position.x), roundfloat(tris[2].v.position.y), color(0, 255, 0));
            draw_bresenham_adjusted(roundfloat(tris[2].v.position.x), roundfloat(tris[2].v.position.y), roundfloat(tris[0].v.position.x), roundfloat(tris[0].v.position.y), color(0, 255, 0));
        }
    }

    void drawTrianglesRasterized(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const mat4f &modelmat) {
        triangles.clear();
        makeRequiredTriangles(vertices, indices, modelmat);
        rasterizeTriangles();
    }

    private:

        
    inline float max(const float &first, const float &second) {
          return first > second ? first : second;
      }

      inline vec3 reflect(const vec3 &I, const vec3 &N) {
          return I - N * 2.0 * vec3::dot(N, I);
      }

      //function to calculate effect of directional light in a fragment with color col
      inline color CalcDirLight(const vec3 &normal, const vec3 &viewdir, const color &col) {
          auto diff = max(vec3::dot(normal, -dirlight.direction), 0.0f);
          auto spec = pow(max(vec3::dot(viewdir, reflect(dirlight.direction, normal)), 0.0), currentMaterial->shininess);
          color diffuse = col * dirlight.col * dirlight.intensity * currentMaterial->DiffuseStrength * diff;
          color specular = dirlight.col * dirlight.intensity * currentMaterial->SpecularStrength * spec;
          diffuse += specular;
          diffuse.a() = 255;
          return std::move(diffuse);
      }

      //function to calculate effect of a point light in a fragment with color col
      color CalcPointLight(const pointLight &light, const vec3 &normal, const vec3 &fragPos, const vec3 &viewDir, const color &diffuseColor, float int_by_at) {
          const vec3 lightDir = vec3::normalize(light.getpos() - fragPos);
          const float diff = max(vec3::dot(normal, lightDir), 0.0);
          const vec3 halfwayDir = vec3::normalize(lightDir + viewDir);
          const float spec = pow(max(vec3::dot(normal, halfwayDir), 0.0), currentMaterial->shininess);
          const color ambient = diffuseColor * light.get_ambient_color() * (currentMaterial->AmbientStrength * int_by_at);
          const color diffuse = diffuseColor * light.get_diffuse_color() * (currentMaterial->DiffuseStrength * diff * int_by_at);
          const color specular = light.get_diffuse_color() * (currentMaterial->SpecularStrength * spec * int_by_at);
          return (ambient + diffuse + specular);
      }

         //putpixel assuming middle of screen to be the origin
      void putpixel_adjusted(int x, int y, float z, const color &col = 255) {
          assert(x < fboCPU->xmax && x > fboCPU->xmin && y < fboCPU->ymax && y > fboCPU->ymin);
          const size_t indx = ((size_t)x + fboCPU->xmax) + ((size_t)y + fboCPU->ymax) * fboCPU->x_size;
          fboCPU->colorlayer[indx] = col;
          fboCPU->grid[indx] = true;
          fboCPU->z[indx] = z;
      }

      //get z value of specified pixel from framebuffer
      inline float getpixelZ_adjusted(int x, int y) const {
          const bool test = ((size_t)x + fboCPU->xmax) < fboCPU->x_size && (x + fboCPU->xmax) >= 0 && ((size_t)y + fboCPU->ymax) < fboCPU->y_size && (y + fboCPU->ymax) >= 0;
          return test ? fboCPU->z[((size_t)x + fboCPU->xmax) + ((size_t)y + fboCPU->ymax) * fboCPU->x_size] : std::numeric_limits<float>::max();
      }

      //bresenham line drawing function to draw wireframe of objects
      void draw_bresenham_adjusted(int x1, int y1, int x2, int y2, const color &col = 0) {
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
    inline void clip1(const std::array<vec4, 3> &tris, unsigned char which, float &u1, float &u2) {
        u1 = -(nearPlane + tris[which].z) / (tris[(which + 1) % 3].z - tris[which].z);
        u2 = -(nearPlane + tris[which].z) / (tris[(which + 2) % 3].z - tris[which].z);
    }

    inline void clip2(const std::array<vec4, 3> &tris, unsigned char idx1, unsigned char idx2, unsigned char rem, float &u1, float &u2) {
        u1 = -(nearPlane + tris[idx1].z) / (tris[rem].z - tris[idx1].z);
        u2 = -(nearPlane + tris[idx2].z) / (tris[rem].z - tris[idx2].z);
    }

    //clips and perspective transforms input triangle vertices if two points are out of clip space
    inline void clip2helper(const mat4f &per, const std::array<vec3, 3> &extraInfoAboutVertex, const std::array<vec4, 3> &modelviewTransformed, Vertex *points, unsigned char idx1, unsigned char idx2, unsigned char rem, std::array<Vertex2, 3> &t) {
        float u1, u2;
        clip2(modelviewTransformed, idx1, idx2, rem, u1, u2);
        t[idx1] = Vertex2(Vertex::perspectiveMul(points[idx1] + (points[rem] - points[idx1]) * u1, per), extraInfoAboutVertex[idx1] + (extraInfoAboutVertex[rem] - extraInfoAboutVertex[idx1]) * u1);
        t[idx2] = Vertex2(Vertex::perspectiveMul(points[idx2] + (points[rem] - points[idx2]) * u2, per), extraInfoAboutVertex[idx2] + (extraInfoAboutVertex[rem] - extraInfoAboutVertex[idx2]) * u2);
        t[rem] = Vertex2(Vertex(modelviewTransformed[rem], points[rem].normal, points[rem].texCoord).perspectiveMul(per), extraInfoAboutVertex[rem]);
    }

    //clips and perspective transforms input triangle vertices if one point is out of the clip space
    inline void clip1helper(const mat4f &per, const std::array<vec3, 3> &extraInfoAboutVertex, const std::array<vec4, 3> &modelviewTransformed, Vertex *points, unsigned char idx1, std::array<Vertex2, 3> &t, std::vector<std::array<Vertex2, 3>> &triangles) {
        float u1, u2;
        unsigned char idx2 = (idx1 + 1) % 3, idx3 = (idx1 + 2) % 3;
        clip1(modelviewTransformed, idx1, u1, u2);

        t[idx1] = Vertex2(Vertex::perspectiveMul(points[idx1] + (points[idx2] - points[idx1]) * u1, per), extraInfoAboutVertex[idx1] + (extraInfoAboutVertex[idx2] - extraInfoAboutVertex[idx1]) * u1);
        t[idx2] = Vertex2(Vertex(modelviewTransformed[idx2], points[idx2].normal, points[idx2].texCoord).perspectiveMul(per), extraInfoAboutVertex[idx2]);
        t[idx3] = Vertex2(Vertex(modelviewTransformed[idx3], points[idx3].normal, points[idx3].texCoord).perspectiveMul(per), extraInfoAboutVertex[idx3]);
        triangles.emplace_back(t);
        t[idx2] = t[idx3];
        t[idx3] = Vertex2(Vertex::perspectiveMul(points[idx1] + (points[idx3] - points[idx1]) * u2, per), extraInfoAboutVertex[idx1] + (extraInfoAboutVertex[idx3] - extraInfoAboutVertex[idx1]) * u2);
        triangles.emplace_back(std::move(t));
    }

    //rasterize BottomFlatTriangle: p0/first_argument is the top unique point
    void fillBottomFlatTriangle(const vec2_T<int> &p0, const vec2_T<int> &p1, const vec2_T<int> &p2, const Vertex2 &v0, const Vertex2 &v1, const Vertex2 &v2) {
        double invslope1 = ((double)p0.x - p1.x) / ((double)p0.y - p1.y);
        double invslope2 = ((double)p0.x - p2.x) / ((double)p0.y - p2.y);

        double currentx1 = p1.x - invslope1, currentx2 = p2.x - invslope2;
        Vertex2 vx1 = v1, vx2 = v2, vx3 = v0;

        Vertex2 diff1 = v0 - v1, diff2 = v0 - v2, diff3;
        double unit1 = 1 / ((double)p0.y - p1.y), unit2 = 1 / (double)((double)p0.y - p2.y), unit3 = 0;
        double u1 = -unit1, u2 = -unit2, u3 = 0;

        for (int scanlineY = p1.y; scanlineY <= p0.y && scanlineY < fboCPU->ymax; ++scanlineY) {
            currentx1 += invslope1;
            u1 += unit1;
            vx1 = v1 + diff1 * u1;

            currentx2 += invslope2;
            u2 += unit2;
            vx2 = v2 + diff2 * u2;

            if (scanlineY <= fboCPU->ymin) {
                continue;
            }

            unit3 = (currentx2 - currentx1) < epsilon ? 0 : 1 / (currentx2 - currentx1);
            u3 = 0;
            diff3 = vx2 - vx1;

            for (int i = currentx1; i <= rounddouble(currentx2) && i < fboCPU->xmax; ++i, u3 += unit3) {
                if (i <= fboCPU->xmin) {
                    continue;
                }

                float z = 1 / (vx1.v.position.z + (diff3.v.position.z) * u3);
                auto gotz = getpixelZ_adjusted(i, scanlineY);
                if (gotz < z) {
                    vx3 = vx1 + diff3 * u3;

                    if (currentMaterial) {
                        putpixel_adjusted(i, scanlineY, z, getcolor(vx3));
                    } else {
                        putpixel_adjusted(i, scanlineY, z, color(255, 0, 0));
                    }
                }
            }
        }
    }

    //rasterize TopFlatTriangle: p2/last_point is the bottom unique point
    void fillTopFlatTriangle(const vec2_T<int> &p0, const vec2_T<int> &p1, const vec2_T<int> &p2, const Vertex2 &v0, const Vertex2 &v1, const Vertex2 &v2) {
        const double invslope1 = ((double)p2.x - p1.x) / ((double)p2.y - p1.y);
        const double invslope2 = ((double)p2.x - p0.x) / ((double)p2.y - p0.y);

        double currentx1 = p1.x + invslope1, currentx2 = p0.x + invslope2;
        Vertex2 vx1 = v1, vx2 = v0, vx3 = v2;

        double unit1 = 1 / ((double)p1.y - p2.y), unit2 = 1 / ((double)p0.y - p2.y), unit3 = 0;
        double u1 = -unit1, u2 = -unit2, u3 = 0;
        Vertex2 diff1 = v2 - v1, diff2 = v2 - v0, diff3;

        for (int scanlineY = p0.y; scanlineY >= p2.y && scanlineY > fboCPU->ymin; --scanlineY) {
            currentx1 = currentx1 - invslope1;
            u1 += unit1;
            vx1 = v1 + diff1 * u1;

            currentx2 = currentx2 - invslope2;
            u2 += unit2;
            vx2 = v0 + diff2 * u2;

            if (scanlineY >= fboCPU->ymax) {
                continue;
            }

            u3 = 0;

            unit3 = (currentx2 - currentx1) < epsilon ? 0 : 1 / (currentx2 - currentx1);
            diff3 = vx2 - vx1;

            for (int i = currentx1; i <= round(currentx2); ++i, u3 += unit3) {
                if (i <= fboCPU->xmin) {
                    continue;
                } else if (scanlineY >= fboCPU->xmax) {
                    break;
                }
                const float z = 1 / (vx1.v.position.z + (diff3.v.position.z) * u3);
                const auto gotz = getpixelZ_adjusted(i, scanlineY);
                if (gotz < z) {
                    vx3 = vx1 + diff3 * u3;
                    if (currentMaterial) {
                        putpixel_adjusted(i, scanlineY, z, getcolor(vx3));
                    } else {
                        putpixel_adjusted(i, scanlineY, z, color(255, 255, 255));
                    }
                }
            }
        }
    }

    //helper function to sort 3 items
    template <class T>
    void sort3Values(T &item0, T &item1, T &item2, std::function<bool(const T &, const T &)> function) {
        // Insert item1
        if (function(item1, item0))
            std::swap(item0, item1);

        // Insert item2
        if (function(item2, item1)) {
            std::swap(item1, item2);
            if (function(item1, item0))
                std::swap(item1, item0);
        }
    }

    void rasterizeTriangles() {
        for (auto &tris : triangles) {

            sort3Values<Vertex2>(tris[0], tris[1], tris[2], [](const Vertex2 &v1, const Vertex2 &v2) { return v1.v.position.y > v2.v.position.y; });
            //std::sort(tris.begin(), tris.end(), [](const Vertex2 &v1, const Vertex2 &v2) { return v1.v.position.y > v2.v.position.y; });

            vec2_T<int> points[3];
            points[0] = vec2_T<int>(roundfloat(tris[0].v.position.x), roundfloat(tris[0].v.position.y));
            points[1] = vec2_T<int>(roundfloat(tris[1].v.position.x), roundfloat(tris[1].v.position.y));
            points[2] = vec2_T<int>(roundfloat(tris[2].v.position.x), roundfloat(tris[2].v.position.y));

            if (points[0].y == points[1].y) // natural flat top
            {
                //sorting top vertices by x
                if (points[1].x > points[0].x) {
                    std::swap(tris[0], tris[1]);
                    std::swap(points[0], points[1]);
                }

                fillTopFlatTriangle(points[0], points[1], points[2], tris[0], tris[1], tris[2]);
                continue;
            } else if (points[1].y == points[2].y) // natural flat bottom
            {
                // sorting bottom vertices by x
                if (points[2].x < points[1].x) {
                    std::swap(tris[1], tris[2]);
                    std::swap(points[1], points[2]);
                }

                fillBottomFlatTriangle(points[0], points[1], points[2], tris[0], tris[1], tris[2]);
                continue;
            } else // general triangle
            {
                // find splitting vertex interpolant
                const double alphaSplit = ((double)points[1].y - points[0].y) / ((double)points[2].y - points[0].y);

                const auto diff = tris[2] - tris[0];
                //checkTexcoords(diff.v.texCoord);
                const auto vi = tris[0] + diff * alphaSplit;

                //checkTexcoords(vi.v.texCoord);
                auto pi = points[0] + (points[2] - points[0]) * alphaSplit;
                pi.y = points[1].y;

                if (points[1].x < pi.x) // major right
                {
                    fillBottomFlatTriangle(points[0], points[1], pi, tris[0], tris[1], vi);
                    fillTopFlatTriangle(pi, points[1], points[2], vi, tris[1], tris[2]);
                } else // major left
                {
                    fillBottomFlatTriangle(points[0], pi, points[1], tris[0], vi, tris[1]);
                    fillTopFlatTriangle(points[1], pi, points[2], tris[1], vi, tris[2]);
                }
            }
        }
        int x = 4;
    }

    //fill intensity information for each vertex needed for gouraud shading
    void fillExtraInformationForGoraudShading(Vertex &v, vec3 &extraInfoAboutVertex) {
        auto viewDir = (cam->eye - extraInfoAboutVertex).normalize();
        for (auto &light : pointLights) {
            float dist = vec3::dist(extraInfoAboutVertex, light.getpos());
            float int_by_at = light.intensity / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
            const vec3 lightDir = vec3::normalize(light.getpos() - extraInfoAboutVertex);
            extraInfoAboutVertex = vec3();
            if (int_by_at > 0.04) {
                const float diff = max(vec3::dot(v.normal, lightDir), 0.0);
                const vec3 halfwayDir = vec3::normalize(lightDir + viewDir);
                const float spec = pow(max(vec3::dot(v.normal, halfwayDir), 0.0), currentMaterial->shininess);
                extraInfoAboutVertex += vec3(currentMaterial->AmbientStrength * int_by_at, currentMaterial->DiffuseStrength * diff * int_by_at, currentMaterial->SpecularStrength * spec * int_by_at);
            }
        }
    }

    void makeRequiredTriangles(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const mat4f &modelmat) {
        auto view = trans::lookAt(cam->eye, cam->eye + cam->getViewDir(), cam->getUp());
        auto per = trans::persp(fboCPU->x_size, fboCPU->y_size, cam->FOV);
        for (size_t i = 0; i < indices.size(); i += 3) {
            std::array<vec4, 3> modelviewTransformed;
            std::array<vec3, 3> extraInfoAboutVertex;
            std::array<bool, 3> clip;
            clip[2] = clip[1] = clip[0] = false;
            Vertex points[3] = {vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]};
            std::array<Vertex2, 3> t;
            // Vertex temp;
            extraInfoAboutVertex[0] = modelmat * points[0].position;
            extraInfoAboutVertex[1] = modelmat * points[1].position;
            extraInfoAboutVertex[2] = modelmat * points[2].position;

            modelviewTransformed[0] = view * (modelmat * points[0].position);
            if (modelviewTransformed[0].z > -nearPlane) {
                clip[0] = true;
            }

            modelviewTransformed[1] = view * (modelmat * points[1].position);
            if (modelviewTransformed[1].z > -nearPlane) {
                clip[1] = true;
            }

            modelviewTransformed[2] = view * (modelmat * points[2].position);
            if (modelviewTransformed[2].z > -nearPlane) {
                clip[2] = true;
            }

            float u1, u2;
            points[0].position = modelviewTransformed[0];
            points[0].normal = mat4mulvec3(modelmat, points[0].normal);
            points[1].position = modelviewTransformed[1];
            points[1].normal = mat4mulvec3(modelmat, points[1].normal);
            points[2].position = modelviewTransformed[2];
            points[2].normal = mat4mulvec3(modelmat, points[2].normal);

            if (cullBackface && vec3::dot(mat4mulvec3(view, points[0].normal), points[0].position) > 0) {
                continue;
            }

#ifndef PHONG_SHADING
            fillExtraInformationForGoraudShading(points[0], extraInfoAboutVertex[0]);
            fillExtraInformationForGoraudShading(points[1], extraInfoAboutVertex[1]);
            fillExtraInformationForGoraudShading(points[2], extraInfoAboutVertex[2]);
#endif

            if (clip[0] && clip[1] && clip[2]) {
                continue;
            } else if (clip[0]) {
                if (clip[1]) {
                    clip2helper(per, extraInfoAboutVertex, modelviewTransformed, points, 0, 1, 2, t);
                    triangles.emplace_back(std::move(t));
                    continue;
                } else if (clip[2]) {
                    clip2helper(per, extraInfoAboutVertex, modelviewTransformed, points, 0, 2, 1, t);
                    triangles.emplace_back(std::move(t));
                    continue;
                } else {
                    clip1helper(per, extraInfoAboutVertex, modelviewTransformed, points, 0, t, triangles);
                    continue;
                }
            } else if (clip[1]) {
                if (clip[2]) {
                    clip2helper(per, extraInfoAboutVertex, modelviewTransformed, points, 1, 2, 0, t);
                    triangles.emplace_back(std::move(t));
                    continue;
                } else if (clip[0]) {
                    clip2helper(per, extraInfoAboutVertex, modelviewTransformed, points, 1, 0, 2, t);
                    triangles.emplace_back(std::move(t));
                    continue;
                } else {
                    clip1helper(per, extraInfoAboutVertex, modelviewTransformed, points, 1, t, triangles);
                    continue;
                }
            } else if (clip[2]) {
                if (clip[0]) {
                    clip2helper(per, extraInfoAboutVertex, modelviewTransformed, points, 2, 0, 1, t);
                    triangles.emplace_back(std::move(t));
                    continue;
                } else if (clip[1]) {
                    clip2helper(per, extraInfoAboutVertex, modelviewTransformed, points, 2, 1,0, t);
                    triangles.emplace_back(std::move(t));
                    continue;
                } else {
                    clip1helper(per, extraInfoAboutVertex, modelviewTransformed, points, 2, t, triangles);
                    continue;
                }
            }

            t[0] = Vertex2(Vertex::perspectiveMul(points[0], per), extraInfoAboutVertex[0]);
            t[1] = Vertex2(Vertex::perspectiveMul(points[1], per), extraInfoAboutVertex[1]);
            t[2] = Vertex2(Vertex::perspectiveMul(points[2], per), extraInfoAboutVertex[2]);

            triangles.emplace_back(std::move(t));
        }
    }
};