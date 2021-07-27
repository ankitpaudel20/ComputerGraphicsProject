#pragma once
#include "GL/glu.h"
#include <utility>
#include <algorithm>
#include "globals.h"
#include "core.h"

struct MyVertex {
    vec3 position;
    vec3 color;
};

bool sortcol(const MyVertex &v1, const MyVertex &v2) {
    return v1.position.y < v2.position.y;
}
struct framebuffer {
    float *zBuffer;
    bool *grid;
    vec3 *color;
    uint32_t x_size, y_size;
    framebuffer(const uint32_t &x, const uint32_t &y) : x_size(x), y_size(y) {
        grid = new bool[x_size * y_size];
        color = new vec3[x_size * y_size];
        zBuffer = new float[x_size * y_size];
        clear();
    }

    ~framebuffer() {
        delete[] grid;
        delete[] color;
        delete[] zBuffer;
    }

    inline void clear() {
        for (uint32_t x = 0; x < x_size * y_size; ++x) {
            grid[x] = false;
            color[x] = 0;
            zBuffer[x] = 1000;
        }
    }
};

class engine {
  private:
    std::vector<uint32_t> indices;
    framebuffer *fboCPU;
    void putpixel(int x, int y, const float zValue, const vec3 &col = 1) {
        if (x < fboCPU->x_size && x >= 0 && y < fboCPU->y_size && y >= 0) {
            if (zValue < fboCPU->zBuffer[x + y * fboCPU->x_size]) {
                fboCPU->color[x + y * fboCPU->x_size] = col;
                fboCPU->zBuffer[x + y * fboCPU->x_size] = zValue;
            }

            fboCPU->grid[x + y * fboCPU->x_size] = true;
        }
    }

    void putpixel_adjusted(int x, int y, const float zValue,
                           const vec3_T<float> &col = 0) {
        putpixel(x + fboCPU->x_size / 2, y + fboCPU->y_size / 2, zValue, col);
    }

    void draw_bresenham_adjusted(const int &x1, const int &y1, const int &x2,
                                 const int &y2, const float &zValue,
                                 const vec3 &color = vec3(1, 0, 0)) {
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
                putpixel_adjusted(x, y, zValue, color);
            else
                putpixel_adjusted(y, x, zValue, color);

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
    void fillBottomFlatTriangle(const MyVertex &v1, const MyVertex &v2,
                                const MyVertex &v3) {

        float invslope1 =
            (v2.position.x - v1.position.x) / (v2.position.y - v1.position.y);
        float invslope2 =
            (v3.position.x - v1.position.x) / (v3.position.y - v1.position.y);

        float zValueSlope1 =
            (v2.position.z - v1.position.z) / (v2.position.y - v1.position.y);
        float zValueSlope2 =
            (v3.position.z - v1.position.z) / (v3.position.y - v1.position.y);

        vec3 colorSlope1 =
            (v2.color - v1.color) / (v2.position.y - v1.position.y);
        vec3 colorSlope2 =
            (v3.color - v1.color) / (v3.position.y - v1.position.y);

        float currentx1 = v1.position.x;
        float currentx2 = v1.position.x;

        float currentz1 = v1.position.z;
        float currentz2 = v1.position.z;

        vec3 currentColor1 = v1.color;
        vec3 currentColor2 = v1.color;

        float alpha = 0;
        vec3 color_alpha = vec3(0);
        for (int scanlineY = v1.position.y; scanlineY <= v2.position.y;
             scanlineY++) {
            if (currentx2 != currentx1) {
                alpha =
                    (currentz2 - currentz1) / ((int)currentx2 - (int)currentx1);
                color_alpha =
                    (currentColor2 - currentColor1) / (currentx2 - currentx1);
            }

            for (int i = (int)currentx1; i <= (int)currentx2; i++) {
                if (gouraud_test) {
                    putpixel_adjusted(
                        i, scanlineY, currentz1 + alpha * (i - (int)currentx1),
                        currentColor1 + color_alpha * (i - (int)currentx1));
                } else {
                    putpixel_adjusted(i, scanlineY, v1.position.z,
                                      currentColor1);
                }
            }
            // draw_bresenham_adjusted((int)currentx1, scanlineY,
            // (int)currentx2,
            //                         scanlineY, zValue, color);
            currentx1 += invslope1;
            currentx2 += invslope2;

            currentz1 += zValueSlope1;
            currentz2 += zValueSlope2;

            currentColor1 += colorSlope1;
            currentColor2 += colorSlope2;
        }
    }

    void fillTopFlatTriangle(const MyVertex &v1, const MyVertex &v2,
                             const MyVertex &v3

    ) {

        float invslope1 =
            (v3.position.x - v1.position.x) / (v3.position.y - v1.position.y);
        float invslope2 =
            (v3.position.x - v2.position.x) / (v3.position.y - v2.position.y);

        float zValueSlope1 =
            (v3.position.z - v1.position.z) / (v3.position.y - v1.position.y);
        float zValueSlope2 =
            (v3.position.z - v2.position.z) / (v3.position.y - v2.position.y);

        vec3 colorSlope1 =
            (v3.color - v1.color) / (v3.position.y - v1.position.y);
        vec3 colorSlope2 =
            (v3.color - v2.color) / (v3.position.y - v2.position.y);

        float currentx1 = v3.position.x;
        float currentx2 = v3.position.x;

        float currentz1 = v3.position.z;
        float currentz2 = v3.position.z;

        vec3 currentColor1 = v3.color;
        vec3 currentColor2 = v3.color;

        float alpha = 0;
        vec3 color_alpha = vec3(0);

        for (int scanlineY = v3.position.y; scanlineY > v1.position.y;
             scanlineY--) {
            if (currentx2 != currentx1) {
                alpha = (currentz2 - currentz1) / (currentx2 - currentx1);
                color_alpha =
                    (currentColor2 - currentColor1) / (currentx2 - currentx1);
            }
            for (int i = (int)currentx1; i <= (int)currentx2; i++) {
                if (gouraud_test) {
                    putpixel_adjusted(
                        i, scanlineY, currentz1 + alpha * (i - (int)currentx1),
                        currentColor1 + color_alpha * (i - (int)currentx1));
                } else {

                    putpixel_adjusted(i, scanlineY, v1.position.z,
                                      currentColor1);
                }
            }
            // draw_bresenham_adjusted((int)currentx1, scputpixel_adjusted(i,
            // scanlineY, currentz1 + i * alpha,
            //                   color[0]);anlineY,
            // (int)currentx2,
            //                         scanlineY, zValue, color);
            currentx1 -= invslope1;
            currentx2 -= invslope2;

            currentz1 -= zValueSlope1;
            currentz2 -= zValueSlope2;

            currentColor1 -= colorSlope1;
            currentColor2 -= colorSlope2;
        }
    }
    void interpolate(const MyVertex &src, const MyVertex &dst, float alpha,
                     MyVertex &temp) {
        temp.position = src.position + (dst.position - src.position) * alpha;
        temp.color = src.color + (dst.color - src.color) * alpha;

        // temp.normal = src.normal + (dst.normal - src.normal) * alpha;
    }

    vec4 interpolate(const vec4 &src, const vec4 &dst, float alpha) {
        return src + (dst - src) * alpha;
    }

  public:
    engine(const uint32_t &x, const uint32_t &y) {
        fboCPU = new framebuffer(x, y);
        glViewport(0, 0, fboCPU->x_size, fboCPU->y_size);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, fboCPU->x_size, 0.0, fboCPU->y_size);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    ~engine() { delete fboCPU; }

    void clear() {
        glClear(GL_COLOR_BUFFER_BIT);
        fboCPU->clear();
    }

    void resizeFrameBuffer(int width, int height) {
        delete fboCPU;
        fboCPU = new framebuffer(width, height);
    }

    void draw() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_POINTS);
        for (GLint x = 0; x < fboCPU->x_size; ++x) {
            for (GLint y = 0; y < fboCPU->y_size; ++y) {
                if (fboCPU->grid[x + y * fboCPU->x_size]) {
                    vec3 &c = fboCPU->color[x + y * fboCPU->x_size];
                    glColor3f(c.x, c.y, c.z);
                    glVertex2i(x, y);
                }
            }
        }
        glEnd();
        glFlush();
    }

    void drawTraingles(const std::vector<vec4> &cube,
                       const std::vector<uint32_t> &indices) {

        for (int i = 0; i < indices.size(); i += 3) {
            /* subtracting 1 because indices are 1 indexd not zero indexed */
            vec4 vertex1 = cube[indices[i]];
            vec4 vertex2 = cube[indices[i + 1]];
            vec4 vertex3 = cube[indices[i + 2]];

            auto zValue = vertex1.z;

            /* this if condition is completely for testing purpose, it helps to
             * check how each traingle are drawn */
            int filter = 1;
            if (filter or (indices[i] == 3 and indices[i + 1] == 8 and
                           indices[i + 2] == 4)) {

                /* camera vanda paxadi paro vane aile lai puraai traingle nai
                 * display nagarne */
                // if (vertex1.w < 0.1 || vertex2.w < 0.1 || vertex3.w < 0.1) {
                //     continue;
                // }

                vec3 normal = vec3::normalize(vec3::cross(
                    vec3(vertex2 - vertex1), vec3(vertex3 - vertex1)));

                /* calculate the normal here and if the normal and camera
                 * direction dot product gives positive the trangle should not
                 * be drawn */
                auto temp = vec3::dot(normal, vertex1);

                if (temp <= 0) {
                    // continue;
                }

                /*  for testing */
                if (!filter) {
                    std::cout << "normal=" << normal;
                    std::cout << "dot=" << temp << std::endl;
                }

                assert(vertex1.w != 0 and vertex2.w != 0 and vertex3.w != 0);

                float window_width = 640;
                float window_height = 480;

                if (vertex1.x > vertex1.w && vertex2.x > vertex2.w &&
                    vertex3.x > vertex3.w) {
                    continue;
                }
                if (vertex1.x < -vertex1.w && vertex2.x < -vertex2.w &&
                    vertex3.x < -vertex3.w) {
                    continue;
                }
                if (vertex1.y > vertex1.w && vertex2.y > vertex2.w &&
                    vertex3.y > vertex3.w) {
                    continue;
                }
                if (vertex1.y < -vertex1.w && vertex2.y < -vertex2.w &&
                    vertex3.y < -vertex3.w) {
                    continue;
                }
                if (vertex1.z > vertex1.w && vertex2.z > vertex2.w &&
                    vertex3.z > vertex3.w) {
                    continue;
                }
                if (vertex1.w < nearPlane && vertex2.w < nearPlane &&
                    vertex3.w < nearPlane) {
                    continue;
                }
                const auto Clip1 = [this, zValue, i](vec4 &v0, vec4 &v1,
                                                     vec4 &v2) {
                    // calculate alpha values for getting adjusted vertices
                    const float alphaA = (nearPlane - v0.w) / (v1.w - v0.w);
                    const float alphaB = (nearPlane - v0.w) / (v2.w - v0.w);
                    // interpolate to get v0a and v0b
                    const auto v0a = interpolate(v0, v1, fabs(alphaA));
                    const auto v0b = interpolate(v0, v2, fabs(alphaB));
                    // draw triangles
                    bar(v0a, v1, v2, zValue, i);
                    bar(v0b, v0a, v2, zValue, i);
                };
                const auto Clip2 = [this, zValue, i](vec4 &v0, vec4 &v1,
                                                     vec4 &v2) {
                    // calculate alpha values for getting adjusted vertices
                    const float alpha0 = (v0.w - nearPlane) / (v2.w - v0.w);
                    const float alpha1 = (v1.w - nearPlane) / (v2.w - v1.w);
                    // interpolate to get v0a and v0b
                    v0 = interpolate(v0, v2, fabs(alpha0));
                    v1 = interpolate(v1, v2, fabs(alpha1));
                    // draw triangles
                    bar(v0, v1, v2, zValue, i);
                };

                if (showTraingle) {
                    std::cout << "nearplane= " << nearPlane << std::endl;
                    std::cout << "vertex1 =[" << vertex1.x << " ," << vertex1.y
                              << ", " << vertex1.z << ", " << vertex1.w
                              << std::endl;
                    std::cout << "vertex2 =[" << vertex1.x << " ," << vertex2.y
                              << ", " << vertex2.z << ", " << vertex2.w
                              << std::endl;
                    std::cout << "vertex3 =[" << vertex3.x << " ," << vertex3.y
                              << ", " << vertex3.z << ", " << vertex3.w
                              << std::endl;
                    showTraingle = false;
                }

                if (vertex1.w < nearPlane) {
                    if (vertex2.w < nearPlane) {
                        Clip2(vertex1, vertex2, vertex3);
                    } else if (vertex3.w < nearPlane) {
                        Clip2(vertex1, vertex3, vertex2);
                    } else {
                        Clip1(vertex1, vertex2, vertex3);
                    }
                } else if (vertex2.w < nearPlane) {
                    if (vertex3.w < nearPlane) {
                        Clip2(vertex2, vertex3, vertex1);
                    } else {
                        Clip1(vertex2, vertex1, vertex3);
                    }
                } else if (vertex3.w < nearPlane) {
                    Clip1(vertex3, vertex1, vertex2);
                } else // no near clipping necessary
                {
                    bar(vertex1, vertex2, vertex3, zValue, i);
                }
            }
        }
    }

    void rasterize(const std::vector<vec4> &cube,
                   const std::vector<uint32_t> &index_buffer,
                   const std::vector<vec4> &normals) {
        indices = index_buffer;
        std::vector<vec3> color(3);

        for (int i = 0; i < indices.size(); i += 3) {
            /* subtracting 1 because indices are 1 indexd not zero indexed */
            vec4 vertex1 = cube[indices[i]];
            vec4 vertex2 = cube[indices[i + 1]];
            vec4 vertex3 = cube[indices[i + 2]];

            /* this if condition is completely for testing purpose, it helps to
             * check how each traingle are drawn */
            int filter = 1;
            if (filter or (indices[i] == 3 and indices[i + 1] == 8 and
                           indices[i + 2] == 4)) {

                /* camera vanda paxadi paro vane aile lai puraai traingle nai
                 * display nagarne */
                // if (vertex1.w < 0.1 || vertex2.w < 0.1 || vertex3.w < 0.1) {
                //     continue;
                // }

                vec3 normal = vec3::normalize(vec3::cross(
                    vec3(vertex2 - vertex1), vec3(vertex3 - vertex1)));

                // vec3 normal(normals[indices[i]]);

                /* calculate the normal here and if the normal and camera
                 * direction dot product gives positive the trangle should not
                 * be drawn */
                auto temp = vec3::dot(normal, vertex1);
                if (temp <= 0) {
                    // continue;
                }

                /*  for testing */
                if (!filter) {
                    std::cout << "normal=" << normal;
                    std::cout << "dot=" << temp << std::endl;
                }

                assert(vertex1.w != 0 and vertex2.w != 0 and vertex3.w != 0);

                if (vertex1.x > vertex1.w && vertex2.x > vertex2.w &&
                    vertex3.x > vertex3.w) {
                    continue;
                }
                if (vertex1.x < -vertex1.w && vertex2.x < -vertex2.w &&
                    vertex3.x < -vertex3.w) {
                    continue;
                }
                if (vertex1.y > vertex1.w && vertex2.y > vertex2.w &&
                    vertex3.y > vertex3.w) {
                    continue;
                }
                if (vertex1.y < -vertex1.w && vertex2.y < -vertex2.w &&
                    vertex3.y < -vertex3.w) {
                    continue;
                }
                if (vertex1.z > vertex1.w && vertex2.z > vertex2.w &&
                    vertex3.z > vertex3.w) {
                    continue;
                }
                if (vertex1.w < nearPlane && vertex2.w < nearPlane &&
                    vertex3.w < nearPlane) {
                    continue;
                }
                const auto Clip1 = [this, normals, i](vec4 &v0, vec4 &v1,
                                                      vec4 &v2) {
                    // calculate alpha values for getting adjusted vertices
                    const float alphaA = (nearPlane - v0.w) / (v1.w - v0.w);
                    const float alphaB = (nearPlane - v0.w) / (v2.w - v0.w);
                    // interpolate to get v0a and v0b
                    const auto v0a = interpolate(v0, v1, fabs(alphaA));
                    const auto v0b = interpolate(v0, v2, fabs(alphaB));
                    // draw triangles
                    foo(v0a, v1, v2, normals, i);
                    foo(v0b, v0a, v2, normals, i);
                };
                const auto Clip2 = [this, normals, i](vec4 &v0, vec4 &v1,
                                                      vec4 &v2) {
                    // calculate alpha values for getting adjusted vertices
                    const float alpha0 = (v0.w - nearPlane) / (v2.w - v0.w);
                    const float alpha1 = (v1.w - nearPlane) / (v2.w - v1.w);
                    // interpolate to get v0a and v0b
                    v0 = interpolate(v0, v2, fabs(alpha0));
                    v1 = interpolate(v1, v2, fabs(alpha1));
                    // draw triangles
                    foo(v0, v1, v2, normals, i);
                };

                if (showTraingle) {
                    std::cout << "nearplane= " << nearPlane << std::endl;
                    std::cout << "vertex1 =[" << vertex1.x << " ," << vertex1.y
                              << ", " << vertex1.z << ", " << vertex1.w
                              << std::endl;
                    std::cout << "vertex2 =[" << vertex1.x << " ," << vertex2.y
                              << ", " << vertex2.z << ", " << vertex2.w
                              << std::endl;
                    std::cout << "vertex3 =[" << vertex3.x << " ," << vertex3.y
                              << ", " << vertex3.z << ", " << vertex3.w
                              << std::endl;
                    showTraingle = false;
                }

                if (vertex1.w < nearPlane) {
                    if (vertex2.w < nearPlane) {
                        Clip2(vertex1, vertex2, vertex3);
                    } else if (vertex3.w < nearPlane) {
                        Clip2(vertex1, vertex3, vertex2);
                    } else {
                        Clip1(vertex1, vertex2, vertex3);
                    }
                } else if (vertex2.w < nearPlane) {
                    if (vertex3.w < nearPlane) {
                        Clip2(vertex2, vertex3, vertex1);
                    } else {
                        Clip1(vertex2, vertex1, vertex3);
                    }
                } else if (vertex3.w < nearPlane) {
                    Clip1(vertex3, vertex1, vertex2);
                } else // no near clipping necessary
                {
                    foo(vertex1, vertex2, vertex3, normals, i);
                }
            }
        }
    }
    void foo(const vec4 &vertex1, const vec4 &vertex2, const vec4 &vertex3,
             const std::vector<vec4> &normals, const int &i) {
        float window_width = 640;
        float window_height = 480;
        std::vector<MyVertex> traingle = std::vector<MyVertex>(3);
        traingle[0].position =
            vec3((int)round(((vertex1.x / vertex1.w) + 1) * window_width / 2 -
                            window_width / 2),
                 (int)round(((vertex1.y / vertex1.w) + 1) * window_height / 2 -
                            window_height / 2),
                 vertex1.w);

        traingle[1].position =
            vec3((int)round(((vertex2.x / vertex2.w) + 1) * window_width / 2 -
                            window_width / 2),
                 (int)round(((vertex2.y / vertex2.w) + 1) * window_height / 2 -
                            window_height / 2),
                 vertex2.w);

        traingle[2].position =
            vec3((int)round(((vertex3.x / vertex3.w) + 1) * window_width / 2 -
                            window_width / 2),
                 (int)round(((vertex3.y / vertex3.w) + 1) * window_height / 2 -
                            window_height / 2),
                 vertex3.w);

        auto color1 = vec3(0, 1, 0);
        auto lightPos = vec3(0, 0, 0);
        vec3 normal = vec3::normalize(
            vec3::cross(vec3(vertex2 - vertex1), vec3(vertex3 - vertex1)));

        auto lightDir1 = vec3::normalize(vec3(vertex1) - lightPos);
        auto lightDir2 = vec3::normalize(vec3(vertex2) - lightPos);
        auto lightDir3 = vec3::normalize(vec3(vertex3) - lightPos);
        // auto a=normal;

        auto intensity1 = lightDir1 * normal;
        auto intensity2 = lightDir2 * normal;
        auto intensity3 = lightDir3 * normal;

        // auto intensity1 = lightDir1 * normals[indices[i]];
        // auto intensity2 = lightDir2 * normals[indices[i + 1]];
        // auto intensity3 = lightDir3 * normals[indices[i + 2]];

        traingle[0].color = color1 * 1;
        traingle[1].color = color1 * 1;
        traingle[2].color = color1 * 1;

        sort(traingle.begin(), traingle.end(), sortcol);

        if (traingle[0].position.y ==
            traingle[1].position.y) // natural flat top
        {
            // sorting top vertices by x
            if (traingle[1].position.x < traingle[0].position.x)
                std::swap(traingle[0], traingle[1]);

            fillTopFlatTriangle(traingle[0], traingle[1], traingle[2]);
        } else if (traingle[1].position.y ==
                   traingle[2].position.y) // natural flat bottom
        {
            // sorting bottom vertices by x
            if (traingle[2].position.x < traingle[1].position.x)
                std::swap(traingle[1], traingle[2]);

            fillBottomFlatTriangle(traingle[0], traingle[1], traingle[2]);
        } else // general triangle
        {
            // find splitting vertex interpolant
            const float alphaSplit =
                (traingle[1].position.y - traingle[0].position.y) /
                (traingle[2].position.y - traingle[0].position.y);

            MyVertex vi;
            interpolate(traingle[0], traingle[2], alphaSplit, vi);

            if (traingle[1].position.x < vi.position.x) // major right
            {

                fillBottomFlatTriangle(traingle[0], traingle[1], vi);
                fillTopFlatTriangle(traingle[1], vi, traingle[2]);
            } else // major leftzValue,
            {

                fillBottomFlatTriangle(traingle[0], vi, traingle[1]);
                fillTopFlatTriangle(vi, traingle[1], traingle[2]);
            }
        }
    }
    void bar(const vec4 &vertex1, const vec4 &vertex2, const vec4 &vertex3,
             const float &zValue, const int &i) {
        float window_width = 640;
        float window_height = 480;
        draw_bresenham_adjusted(
            (int)round(((vertex1.x / vertex1.w) + 1) * window_width / 2 -
                       window_width / 2),
            (int)round(((vertex1.y / vertex1.w) + 1) * window_height / 2 -
                       window_height / 2),
            (int)round(((vertex2.x / vertex2.w) + 1) * window_width / 2 -
                       window_width / 2),
            (int)round(((vertex2.y / vertex2.w) + 1) * window_height / 2 -
                       window_height / 2),
            zValue);

        draw_bresenham_adjusted(
            (int)round(((vertex2.x / vertex2.w) + 1) * window_width / 2 -
                       window_width / 2),
            (int)round(((vertex2.y / vertex2.w) + 1) * window_height / 2 -
                       window_height / 2),
            (int)round(((vertex3.x / vertex3.w) + 1) * window_width / 2 -
                       window_width / 2),
            (int)round(((vertex3.y / vertex3.w) + 1) * window_height / 2 -
                       window_height / 2),
            zValue);

        draw_bresenham_adjusted(
            (int)round(((vertex1.x / vertex1.w) + 1) * window_width / 2 -
                       window_width / 2),
            (int)round(((vertex1.y / vertex1.w) + 1) * window_height / 2 -
                       window_height / 2),
            (int)round(((vertex3.x / vertex3.w) + 1) * window_width / 2 -
                       window_width / 2),
            (int)round(((vertex3.y / vertex3.w) + 1) * window_height / 2 -
                       window_height / 2),
            zValue);
    }
};
