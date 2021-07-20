#pragma once
#include "core.h"
#include <utility>
#include <list>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct color {
    uint32_t col;
    color() : col(0) {
        int y = 1;
        //printf("defaultused\n");
    }
    color(uint8_t r) : color(r, r, r) {
        int y = 1;
    }
    color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : col((a << 24u) | (b << 16u) | (g << 8u) | r) {
        int y = 1;
        //printf("nondefaultused\n");
    }
    vec4 getcolor() {
        auto temp = (uint8_t *)&col;
        return vec4(temp[0], temp[1], temp[2], temp[3]) / 255;
    }
    uint8_t &r() {
        return ((uint8_t *)&col)[0];
    }
    uint8_t &g() {
        return ((uint8_t *)&col)[1];
    }
    uint8_t &b() {
        return ((uint8_t *)&col)[2];
    }
    uint8_t &a() {
        return ((uint8_t *)&col)[3];
    }
};

struct framebuffer {
    bool *grid;
    color *colorlayer;
    unsigned int x_size, y_size;
    framebuffer(const unsigned int &x, const unsigned int &y) : x_size(x), y_size(y) {
        grid = new bool[x_size * y_size];
        colorlayer = new color[x_size * y_size];
        clear();
    }

    ~framebuffer() {
        delete[] grid;
        delete[] colorlayer;
    }

    inline void clear() {
        for (unsigned int x = 0; x < x_size * y_size; ++x) {
            grid[x] = false;
            colorlayer[x] = color(0, 0, 0, 255);
        }
    }
};

struct engine {

    bool written = false;
    framebuffer *fboCPU;
    float xlim, ylim;

#ifdef NEWRENDERMETHOD
    unsigned int vao, vbo, ibo, tex, shader;

    const char *vertexShaderSource = R"(#version 330 core
    layout (location = 0) in vec3 aPos;
    out vec2 f_texCoord;
    void main()
    {
       	gl_Position = vec4(aPos.xyz, 1.0);
		f_texCoord=aPos.xy;
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
    }
    )";

    float vertices[12] = {
        //    x      y      z
        -1.0f, -1.0f, -0.0f, 1.0f, 1.0f, -0.0f,
        -1.0f, 1.0f, -0.0f, 1.0f, -1.0f, -0.0f};

    unsigned int indices[6] = {
        //  2---,1
        //  | .' |
        //  0'---3
        0, 1, 2, 0, 3, 1};
#endif

    // std::vector<drawable<Vertex>*> draw_queue;

    typedef void (*TransFunc)();

    engine(const unsigned int &x, const unsigned int &y) {
        fboCPU = new framebuffer(x, y);
        xlim = fboCPU->x_size / 2;
        ylim = fboCPU->y_size / 2;

#ifdef NEWRENDERMETHOD
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ibo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        GLcall(glGenTextures(1, &tex));
        // GLcall(glActiveTexture(GL_TEXTURE0 ));
        GLcall(glBindTexture(GL_TEXTURE_2D, tex));
        stbi_set_flip_vertically_on_load(1);
        int m_width, m_height, m_BPP;

#ifdef _MSC_VER
        unsigned char *m_LocalBuffer = stbi_load("output.bmp", &m_width, &m_height, &m_BPP, 3);
#else
        unsigned char *m_LocalBuffer = stbi_load("../res/container2.png", &m_width, &m_height, &m_BPP, 4);
#endif // MSVER
        if (!m_LocalBuffer) {
            std::cout << "texture file unable to load" << std::endl;
            ASSERT(false);
        }

        GLcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_LocalBuffer));
        stbi_image_free(m_LocalBuffer);
        //GLcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboCPU->x_size, fboCPU->y_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, (uint8_t*)fboCPU->colorlayer));

        glGenerateMipmap(GL_TEXTURE_2D);
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
        glUniform1i(location, 1);
        glUseProgram(0);

#else
        glViewport(0, 0, fboCPU->x_size, fboCPU->y_size);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, fboCPU->x_size, 0.0, fboCPU->y_size);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
#endif // NEWRENDERMETHOD
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
    }

    ~engine() {
        delete fboCPU;
#ifdef NEWRENDERMETHOD
        GLcall(glDeleteBuffers(1, &vbo));
        GLcall(glDeleteVertexArrays(1, &vao));
        GLcall(glDeleteTextures(1, &tex));
        GLcall(glDeleteProgram(shader));
#endif
    }

    void clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        fboCPU->clear();
    }

    void draw() {
#ifdef NEWRENDERMETHOD
        if (!written) {
            if (!stbi_write_bmp("output.bmp", fboCPU->x_size, fboCPU->y_size, 4, fboCPU->colorlayer)) {
                printf("write unsueccessful\n");
            };
            written = true;
        }
        GLcall(glUseProgram(shader));
        GLcall(glUniform1i(glGetUniformLocation(shader, "tex"), 0));
        GLcall(glActiveTexture(GL_TEXTURE0));
        GLcall(glBindTexture(GL_TEXTURE_2D, tex));
        //		if (!stbi_write_bmp("output.bmp", fboCPU->x_size, fboCPU->y_size, 4, fboCPU->colorlayer)) {
        //			printf("write unsueccessful\n");
        //		};
        //		int m_width, m_height, m_BPP;
        //#ifdef _MSC_VER
        //		unsigned char* m_LocalBuffer = stbi_load("output.bmp", &m_width, &m_height, &m_BPP, 4);
        //#else
        //		unsigned char* m_LocalBuffer = stbi_load("../res/container2.png", &m_width, &m_height, &m_BPP, 4);
        //#endif // MSVER
        //		if (!m_LocalBuffer) {
        //			std::cout << "texture file unable to load" << std::endl;
        //			ASSERT(false);
        //		}
        /*	glDeleteTextures(1, &tex);
					glGenTextures(1, &tex);
					GLcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboCPU->x_size, fboCPU->y_size, 0, GL_RGBA, GL_UNSIGNED_INT, fboCPU->colorlayer));*/
        //GLcall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fboCPU->x_size, fboCPU->y_size, GL_RGB, GL_UNSIGNED_BYTE, m_LocalBuffer));
        //stbi_image_free(m_LocalBuffer);
        GLcall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fboCPU->x_size, fboCPU->y_size, GL_RGBA, GL_UNSIGNED_BYTE, (uint8_t *)fboCPU->colorlayer));
        GLcall(glBindVertexArray(vao));
        GLcall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

#else
        glBegin(GL_POINTS);
        for (GLint x = 0; x < fboCPU->x_size; ++x) {
            for (GLint y = 0; y < fboCPU->y_size; ++y) {
                if (fboCPU->grid[x + y * fboCPU->x_size]) {
                    glColor4ubv((unsigned char *)(&fboCPU->colorlayer[x + y * fboCPU->x_size].col));
                    glVertex2i(x, y);
                }
            }
        }
        glEnd();
        glFlush();
#endif
    }

    inline void putpixel(int x, int y, const color &col = 255) {
        if (x < fboCPU->x_size && x >= 0 && y < fboCPU->y_size && y >= 0) {
            fboCPU->colorlayer[x + y * fboCPU->x_size] = col;
            fboCPU->grid[x + y * fboCPU->x_size] = true;
        }
    }

    inline void putpixel_adjusted(int x, int y, const color &col = 255) {
        putpixel(x + fboCPU->x_size / 2, y + fboCPU->y_size / 2, col);
    }

    void draw_bresenham_adjusted(int x1, int y1, int x2, int y2, const color &color = 255) {
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
                putpixel_adjusted(x, y, color);
            else
                putpixel_adjusted(y, x, color);

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

    void drawLines(const std::vector<vec2_T<int>> &points, const color &color = 255, const std::vector<unsigned int> &indices = std::vector<unsigned int>()) {
        if (indices.empty()) {
            for (size_t i = 0; i < points.size(); i += 2) {
                draw_bresenham_adjusted(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y, color);
            }
            return;
        }

        for (size_t i = 0; i < indices.size(); i += 2) {
            draw_bresenham_adjusted(points[indices[i]].x, points[indices[i]].y, points[indices[i + 1]].x, points[indices[i + 1]].y, color);
        }
    }

    void drawLinestrip(const std::vector<vec2_T<int>> &points, const color &color = 255, const std::vector<unsigned int> &indices = std::vector<unsigned int>()) {
        for (size_t i = 0; i < points.size(); i++) {
            draw_bresenham_adjusted(points[i].x, points[i].y, points[i + 1].x,
                                    points[i + 1].y, color);
        }
    }

    struct Vertex2 {
        Vertex v;
        vec3 fragpos;
    };
    std::list<std::array<Vertex2, 3>> triangles;
    mat4 viewproj = mat4();

    Vertex2 subVertex2(const Vertex2 &fir, const Vertex2 &sec) {
        Vertex2 temp;
        temp.v = fir.v - sec.v;
        temp.fragpos = fir.fragpos - sec.fragpos;
        return std::move(temp);
    }
    void interpolateVertex(const Vertex2 &fir, const Vertex2 &sec, Vertex2 &mid) {
        auto u = (mid.v.position.x - fir.v.position.x) / (sec.v.position.x - fir.v.position.x);
        // mid.v.position.z = fir.v.position.z + u * (sec.v.position.z - fir.v.position.z);
        mid.fragpos = fir.fragpos + (sec.fragpos - fir.fragpos) * u;
        mid.v = fir.v + (sec.v - fir.v) * u;
    }

    void drawTriangles3d(const std::vector<Vertex> &points, const std::vector<unsigned int> &indices, const mat4 &model = mat4()) {
        std::array<Vertex2, 3> temp;
        for (size_t i = 0; i < indices.size(); i += 3) {
            auto test = model * points[indices[i]].normal;
            if (test.z < 0)
                continue;
            temp[0].v = points[indices[i]];
            temp[0].fragpos = model * temp[0].v.position;
            temp[0].v.position = viewproj * temp[0].fragpos;
            temp[0].v.normal = test;
            temp[1].v = points[indices[i + 1]];
            temp[1].fragpos = model * temp[1].v.position;
            temp[1].v.position = viewproj * temp[1].fragpos;
            temp[1].v.normal = temp[0].v.normal;
            temp[2].v = points[indices[i + 2]];
            temp[2].fragpos = model * temp[2].v.position;
            temp[2].v.position = viewproj * temp[2].fragpos;
            temp[2].v.normal = temp[0].v.normal;
            triangles.emplace_back(temp);
        }

        std::list<std::array<Vertex2, 3>>::iterator i = triangles.begin();
        std::list<std::array<Vertex2, 3>> newTriangles;
    }

    bool sameside(const vec3 &p1, const vec3 &p2, const vec3 &a, const vec3 &b) {
        auto cp1 = vec3::cross(b - a, p1 - a);
        auto cp2 = vec3::cross(b - a, p2 - a);
        if (vec3::dot(cp1, cp2) >= 0)
            return true;
        return false;
    }
    bool PointInTriangle(const vec3 &p, const vec3 &a, const vec3 &b, const vec3 &c) {
        if (sameside(p, a, b, c) && sameside(p, b, a, c) && sameside(p, c, a, b))
            return true;
        return false;
    }

    std::vector<Vertex> rasterizeTriangle(const std::array<Vertex2, 3> &t) {
        const Vertex *top = &t[0].v, *middle = &t[1].v, *bottom = &t[2].v;
        if (middle->position.y < bottom->position.y)
            std::swap(middle, bottom);
        if (top->position.y < middle->position.y)
            std::swap(top, middle);
        if (middle->position.y < bottom->position.y)
            std::swap(middle, bottom);
    }

    std::vector<Vertex> rasterizedPoints;
    void flushTriangles() {
        std::vector<std::array<Vertex2, 3>> triangles;
    }
};