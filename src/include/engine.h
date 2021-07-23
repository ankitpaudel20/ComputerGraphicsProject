#pragma once
#include "GL/glu.h"
#include <utility>
#include <algorithm>

bool sortcol(const vec2 &v1, const vec2 &v2) { return v1.y < v2.y; }
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
    framebuffer *fboCPU;
    void putpixel(int x, int y, const float &zValue, const vec3 &col = 1) {
        if (x < fboCPU->x_size && x >= 0 && y < fboCPU->y_size && y >= 0) {
            if (zValue < fboCPU->zBuffer[x + y * fboCPU->x_size]) {
                fboCPU->color[x + y * fboCPU->x_size] = col;
                fboCPU->zBuffer[x + y * fboCPU->x_size] = zValue;
            }

            fboCPU->grid[x + y * fboCPU->x_size] = true;
        }
    }

    void putpixel_adjusted(int x, int y, const float &zValue,
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
    void fillBottomFlatTriangle(const vec3 &v1, const vec3 &v2, const vec3 &v3,
                                const std::vector<float> &zValue,
                                const std::vector<vec3> &color) {
        float invslope1 = (v2.x - v1.x) / (v2.y - v1.y);
        float invslope2 = (v3.x - v1.x) / (v3.y - v1.y);

        float zValueSlope1 = (zValue[1] - zValue[0]) / (v2.y - v1.y);
        float zValueSlope2 = (zValue[2] - zValue[0]) / (v3.y - v1.y);

        vec3 colorSlope1 = (color[1] - color[0]) / (v2.y - v1.y);
        vec3 colorSlope2 = (color[2] - color[0]) / (v3.y - v1.y);

        float currentx1 = v1.x;
        float currentx2 = v1.x;

        float currentz1 = zValue[0];
        float currentz2 = zValue[0];

        vec3 color1 = color[0];
        vec3 color2 = color[0];

        float alpha = 0;
        vec3 color_alpha = vec3(0);
        for (int scanlineY = v1.y; scanlineY <= v2.y; scanlineY++) {
            if (currentx2 != currentx1) {
                alpha = (currentz2 - currentz1) / (currentx2 - currentx1);
                color_alpha = (color2 - color1) / (currentx2 - currentx1);
            }

            for (int i = (int)currentx1; i <= (int)currentx2; i++) {
                putpixel_adjusted(i, scanlineY, currentz1 + i * alpha,
                                  color1 + color_alpha * i);
                // putpixel_adjusted(i, scanlineY, zValue[0], color[0]);
            }
            // draw_bresenham_adjusted((int)currentx1, scanlineY,
            // (int)currentx2,
            //                         scanlineY, zValue, color);
            currentx1 += invslope1;
            currentx2 += invslope2;

            currentz1 += zValueSlope1;
            currentz2 += zValueSlope2;

            color1 += colorSlope1;
            color2 += colorSlope2;
        }
    }

    void fillTopFlatTriangle(const vec3 &v1, const vec3 &v2, const vec3 &v3,
                             const std::vector<float> &zValue,
                             const std::vector<vec3> &color) {

        float invslope1 = (v3.x - v1.x) / (v3.y - v1.y);
        float invslope2 = (v3.x - v2.x) / (v3.y - v2.y);

        float zValueSlope1 = (zValue[2] - zValue[0]) / (v3.y - v1.y);
        float zValueSlope2 = (zValue[2] - zValue[1]) / (v3.y - v2.y);

        vec3 colorSlope1 = (color[2] - color[0]) / (v3.y - v1.y);
        vec3 colorSlope2 = (color[2] - color[1]) / (v3.y - v2.y);

        float currentx1 = v3.x;
        float currentx2 = v3.x;

        float currentz1 = zValue[2];
        float currentz2 = zValue[2];

        vec3 color1 = color[2];
        vec3 color2 = color[2];

        float alpha = 0;
        vec3 color_alpha = vec3(0);

        for (int scanlineY = v3.y; scanlineY > v1.y; scanlineY--) {
            if (currentx2 != currentx1) {
                alpha = (currentz2 - currentz1) / (currentx2 - currentx1);
                color_alpha = (color2 - color1) / (currentx2 - currentx1);
            }
            for (int i = (int)currentx1; i <= (int)currentx2; i++) {
                putpixel_adjusted(i, scanlineY, currentz1 + i * alpha,
                                  color1 + color_alpha * i);
                // putpixel_adjusted(i, scanlineY, zValue[0], color[0]);
            }
            // draw_bresenham_adjusted((int)currentx1, scanlineY,
            // (int)currentx2,
            //                         scanlineY, zValue, color);
            currentx1 -= invslope1;
            currentx2 -= invslope2;

            currentz1 -= zValueSlope1;
            currentz2 -= zValueSlope2;

            color1 -= colorSlope1;
            color2 -= colorSlope2;
        }
    }
    vec3 interpolate(const vec2 &src, const vec2 &dst, float alpha) {
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
                if (vertex1.w < 0.1 || vertex2.w < 0.1 || vertex3.w < 0.1) {
                    continue;
                }

                vec3 normal = vec3::normalize(vec3::cross(
                    vec3(vertex2 - vertex1), vec3(vertex3 - vertex1)));

                /* calculate the normal here and if the normal and camera
                 * direction dot product gives positive the trangle should not
                 * be drawn */
                auto temp = vec3::dot(normal, vertex1);

                if (temp <= 0) {
                    continue;
                }

                /*  for testing */
                if (!filter) {
                    std::cout << "normal=" << normal;
                    std::cout << "dot=" << temp << std::endl;
                }

                assert(vertex1.w != 0 and vertex2.w != 0 and vertex3.w != 0);

                float window_width = 640;
                float window_height = 480;

                draw_bresenham_adjusted(
                    (int)round(((vertex1.x / vertex1.w) + 1) * window_width /
                                   2 -
                               window_width / 2),
                    (int)round(((vertex1.y / vertex1.w) + 1) * window_height /
                                   2 -
                               window_height / 2),
                    (int)round(((vertex2.x / vertex2.w) + 1) * window_width /
                                   2 -
                               window_width / 2),
                    (int)round(((vertex2.y / vertex2.w) + 1) * window_height /
                                   2 -
                               window_height / 2),
                    zValue);

                draw_bresenham_adjusted(
                    (int)round(((vertex2.x / vertex2.w) + 1) * window_width /
                                   2 -
                               window_width / 2),
                    (int)round(((vertex2.y / vertex2.w) + 1) * window_height /
                                   2 -
                               window_height / 2),
                    (int)round(((vertex3.x / vertex3.w) + 1) * window_width /
                                   2 -
                               window_width / 2),
                    (int)round(((vertex3.y / vertex3.w) + 1) * window_height /
                                   2 -
                               window_height / 2),
                    zValue);

                draw_bresenham_adjusted(
                    (int)round(((vertex1.x / vertex1.w) + 1) * window_width /
                                   2 -
                               window_width / 2),
                    (int)round(((vertex1.y / vertex1.w) + 1) * window_height /
                                   2 -
                               window_height / 2),
                    (int)round(((vertex3.x / vertex3.w) + 1) * window_width /
                                   2 -
                               window_width / 2),
                    (int)round(((vertex3.y / vertex3.w) + 1) * window_height /
                                   2 -
                               window_height / 2),
                    zValue);
            }
        }
    }

    void rasterize(const std::vector<vec4> &cube,
                   const std::vector<uint32_t> &indices) {

        std::vector<float> zValue(3);
        std::vector<vec3> color(3);
        for (int i = 0; i < indices.size(); i += 3) {
            /* subtracting 1 because indices are 1 indexd not zero indexed */
            vec4 vertex1 = cube[indices[i]];
            vec4 vertex2 = cube[indices[i + 1]];
            vec4 vertex3 = cube[indices[i + 2]];

            zValue[0] = vertex1.w;
            zValue[1] = vertex2.w;
            zValue[2] = vertex3.w;

            /* this if condition is completely for testing purpose, it helps to
             * check how each traingle are drawn */
            int filter = 1;
            if (filter or (indices[i] == 7 and indices[i + 1] == 6 and
                           indices[i + 2] == 8)) {

                /* camera vanda paxadi paro vane aile lai puraai traingle nai
                 * display nagarne */
                if (vertex1.w < 0.1 || vertex2.w < 0.1 || vertex3.w < 0.1) {
                    continue;
                }

                vec3 normal = vec3::normalize(vec3::cross(
                    vec3(vertex2 - vertex1), vec3(vertex3 - vertex1)));

                /* calculate the normal here and if the normal and camera
                 * direction dot product gives positive the trangle should not
                 * be drawn */
                auto temp = vec3::dot(normal, vertex1);
                if (temp <= 0) {
                    continue;
                }

                /*  for testing */
                if (!filter) {
                    std::cout << "normal=" << normal;
                    std::cout << "dot=" << temp << std::endl;
                }

                assert(vertex1.w != 0 and vertex2.w != 0 and vertex3.w != 0);

                float window_width = 640;
                float window_height = 480;

                std::vector<vec2> traingle = std::vector<vec2>(3);
                traingle[0] = vec2((int)round(((vertex1.x / vertex1.w) + 1) *
                                                  window_width / 2 -
                                              window_width / 2),
                                   (int)round(((vertex1.y / vertex1.w) + 1) *
                                                  window_height / 2 -
                                              window_height / 2));

                traingle[1] = vec2((int)round(((vertex2.x / vertex2.w) + 1) *
                                                  window_width / 2 -
                                              window_width / 2),
                                   (int)round(((vertex2.y / vertex2.w) + 1) *
                                                  window_height / 2 -
                                              window_height / 2));

                traingle[2] = vec2((int)round(((vertex3.x / vertex3.w) + 1) *
                                                  window_width / 2 -
                                              window_width / 2),
                                   (int)round(((vertex3.y / vertex3.w) + 1) *
                                                  window_height / 2 -
                                              window_height / 2));

                auto color1 = vec3(1, 1, 1);
                auto lightPos = vec3(0, 0, 0);
                auto lightDir1 = vec3::normalize(vec3(vertex1) - lightPos);
                auto lightDir2 = vec3::normalize(vec3(vertex2) - lightPos);
                auto lightDir3 = vec3::normalize(vec3(vertex3) - lightPos);
                // auto a=normal;
                auto intensity1 = lightDir1 * normal;
                auto intensity2 = lightDir2 * normal;
                auto intensity3 = lightDir3 * normal;
                // if (intensity < 0) {
                //     intensity = 0;
                // }
                color[0] = color1 * intensity1;
                color[1] = color1 * intensity2;
                color[2] = color1 * intensity3;
                // if (i % 2 == 0) {
                //     color = vec3(0, 1, 0);
                // }

                sort(traingle.begin(), traingle.end(), sortcol);
                if (traingle[0].y == traingle[1].y) // natural flat top
                {
                    // sorting top vertices by x
                    if (traingle[1].x < traingle[0].x)
                        std::swap(traingle[0], traingle[1]);

                    fillTopFlatTriangle(traingle[0], traingle[1], traingle[2],
                                        zValue, color);
                } else if (traingle[1].y ==
                           traingle[2].y) // natural flat bottom
                {
                    // sorting bottom vertices by x
                    if (traingle[2].x < traingle[1].x)
                        std::swap(traingle[1], traingle[2]);

                    fillBottomFlatTriangle(traingle[0], traingle[1],
                                           traingle[2], zValue, color);
                } else // general triangle
                {
                    // find splitting vertex interpolant
                    const float alphaSplit = (traingle[1].y - traingle[0].y) /
                                             (traingle[2].y - traingle[0].y);
                    const auto vi = interpolate(vec2(traingle[0]),
                                                vec2(traingle[2]), alphaSplit);

                    if (traingle[1].x < vi.x) // major right
                    {
                        fillBottomFlatTriangle(traingle[0], traingle[1], vi,
                                               zValue, color);
                        fillTopFlatTriangle(traingle[1], vi, traingle[2],
                                            zValue, color);
                    } else // major leftzValue,
                    {
                        fillBottomFlatTriangle(traingle[0], vi, traingle[1],
                                               zValue, color);
                        fillTopFlatTriangle(vi, traingle[1], traingle[2],
                                            zValue, color);
                    }
                }
            }
        }
    }
};