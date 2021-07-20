#include <chrono>
#include <iostream>
#include <vector>

#include "GL/glew.h"
#include "GL/glu.h"
#include "GLFW/glfw3.h"
#include "camera.h"
#include "engine.h"
#include "model.h"
#include "transformations.h"

#include <filesystem>

namespace fs = std::filesystem;

mat4 translate3d;
mat4 scale3d;
camera cam;
uint32_t deltatime;

// { {1.000000, 1.000000, -1.000000,1.0}
//  {1.000000, -1.000000, -1.000000,1.0}
//  {1.000000, 1.000000, 1.000000,1.}
//  {1.000000, -1.000000, 1.000000,1.0}
//  {-1.000000, 1.000000, -1.000000,1.0}
//  {-1.000000, -1.000000, -1.00000,1.0}
//  {-1.000000, 1.000000, 1.000000,1.0}
//  {-1.000000, -1.000000, 1.000000,1.0}}

//  {
//  5, 3, 1,
//  3, 8, 4,
//  7, 6, 8,
//  2, 8, 6,
//  1, 4, 2,
//  5, 2, 6,
//  5, 7, 3,
//  3, 7, 8,
//  7, 5, 6,
//  2, 4, 8,
//  1, 3, 4,
//  5, 1, 2}

/* const std::vector<vec4> cube = {{-1, -1, -1, 1}, {-1, -1, 1, 1}, {-1, 1, 1,
1},
                                {-1, 1, -1, 1},  {1, -1, -1, 1}, {1, -1, 1, 1},
                                {1, 1, 1, 1},    {1, 1, -1, 1}};

const std::vector<uint32_t> cube_indices = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                                            6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
*/

const std::vector<vec4> cube = {{1.000000, 1.000000, -1.000000, 1.0},
                                {1.000000, -1.000000, -1.000000, 1.0},
                                {1.000000, 1.000000, 1.000000, 1.},
                                {1.000000, -1.000000, 1.000000, 1.0},
                                {-1.000000, 1.000000, -1.000000, 1.0},
                                {-1.000000, -1.000000, -1.00000, 1.0},
                                {-1.000000, 1.000000, 1.000000, 1.0},
                                {-1.000000, -1.000000, 1.000000, 1.0}};

const std::vector<uint32_t> cube_indices = {5, 3, 1, 3, 8, 4, 7, 6, 8, 2, 8, 6,
                                            1, 4, 2, 5, 2, 6, 5, 7, 3, 3, 7, 8,
                                            7, 5, 6, 2, 4, 8, 1, 3, 4, 5, 1, 2};

const std::vector<vec4> axes_endpoints = {{-100, 0, 0, 1}, {100, 0, 0, 1},
                                          {0, 100, 0, 1},  {0, -100, 0, 1},
                                          {0, 0, 100, 1},  {0, 0, -100, 1}};

float zvp = 50, zprp = 200;

static void error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
}

static bool captured = false;
static int mx, my;

std::string searchRes() {
    auto currentPath = fs::current_path();

    auto orgiPath = currentPath;
    bool resPathFound = false, appPathFound = false;

    while (true) {
        if (!resPathFound && !appPathFound && currentPath.has_parent_path()) {
            fs::current_path(currentPath.parent_path());
            currentPath = fs::current_path();
        } else if (appPathFound) {
            try {
#ifdef _WIN_32
                auto a = currentPath.string().append("\\res");
#else
                auto a = currentPath.string().append("res");
#endif
                fs::current_path(a);
                fs::current_path(orgiPath);
                return a;
            } catch (const std::exception &e) {
                std::cout << e.what()
                          << "\n \"res\" folder not found inside "
                             "\"Application\" folder \n";
                return std::string();
            }
        } else
            break;

        // std::cout << "currently in path: " << currentPath.string() <<
        // std::endl;

        for (auto &dirs : fs::directory_iterator(currentPath)) {
            if (dirs.is_directory() &&
                dirs.path().filename().string() == "src") {
                appPathFound = true;
                break;
            }
        }
    }

    std::cout << "folder \"Application\" not found in 4 back iterations"
              << std::endl;
    DEBUG_BREAK;
    return std::string();
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
    // auto dir = cam.center - cam.eye;
    // const float cameraSpeed = 2.5 * s.deltatime;
    vec3 left = vec3::cross(cam.getUp(), cam.getViewDir());
    switch (action) {
    case GLFW_PRESS:
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_1:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            captured = false;
            break;
        case GLFW_KEY_2:
            glfwSetCursorPos(window, mx, my);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            captured = true;
            break;
        default:
            break;
        }
        break;
    }
}

float xy_scale = 1;
float z_scale = 1;
void processHoldEvent(GLFWwindow *window) {
    // auto dir = cam.center - cam.eye;

    const float cameraSpeed = cam.speed * deltatime / 100;
    vec3 left = vec3::normalize(vec3::cross(cam.getUp(), cam.getViewDir()));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cam.eye += (cam.getViewDir() * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cam.eye += (left * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cam.eye -= (cam.getViewDir() * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cam.eye -= (left * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        cam.eye += (cam.getUp() * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        cam.eye -= (cam.getUp() * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        xy_scale += 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        xy_scale += -0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        z_scale += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        z_scale -= 0.01;
    }

    float factor = 100;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        cam.DelPitch(cam.sensitivity * deltatime);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        cam.DelPitch(-cam.sensitivity * deltatime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        cam.newDelYaw(-cam.sensitivity * deltatime * factor);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        cam.newDelYaw(cam.sensitivity * deltatime * factor);
    }
}

static void cursor_position_callback(GLFWwindow *window, double x, double y) {
    // printf("(x=%d, y=%d)\n", x, y);
    float factor = 100;
    if (captured) {
        float xoffset = mx - x;
        float yoffset = my - y;
        mx = x;
        my = y;

        float sensitivity = 0.09;
        xoffset *= cam.sensitivity * deltatime;
        yoffset *= cam.sensitivity * deltatime;

        cam.newDelYaw(xoffset);
        cam.DelPitch(yoffset);
    }
}

int window_width, window_height;
engine *graphicsEngine;

static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
    // auto oldWidth = window_width;
    // auto oldHeight = window_height;

    window_height = height;
    window_width = width;

    delete graphicsEngine->fboCPU;
    graphicsEngine->fboCPU = new framebuffer(width, height);

    glViewport(0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, window_width, 0.0, window_height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv) {

    GLFWwindow *window;

    if (!glfwInit())
        return -1;
    glfwSetErrorCallback(error_callback);

    window = glfwCreateWindow(640, 480, "main", NULL, NULL);
    if (!(window)) {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
    }

    glfwSwapInterval(1);

    glfwGetFramebufferSize(window, &window_width, &window_height);
    graphicsEngine = new engine(window_width, window_height);

    std::string path = "../res";
    // auto path = searchRes();

    auto arrow = Model::loadModel_obj(path + "/arrow.obj", "arrow");

    float rotation_angle = 0;
    float view_angle = 0.0;
    float angle_rotated = 0;
    float zvp = 50, zprp = -500;

    auto lastframe = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        deltatime = std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::high_resolution_clock::now() - lastframe)
                        .count();
        lastframe = std::chrono::high_resolution_clock::now();

        glfwPollEvents();
        processHoldEvent(window);
        rotation_angle += 1;
        view_angle += 0.1;
        if (rotation_angle > 360) {
            rotation_angle = 0;
        }
        if (view_angle > 180) {
            view_angle = 1;
        }
        // zprp += 0.1;
        // printf("zprp=%f\n", zprp);

        // translate3d = trans::translation(vec3(-1, -1, -1));
        scale3d = trans::scaling3d(vec3(xy_scale, xy_scale, z_scale));
        auto rotate_y = trans::y_rotation(rotation_angle);
        std::vector<vec2_T<int>> final_cube, final_axes;

        std::cout << "camera Eye: " << cam.eye << std::endl;
        std::cout << "camera up: " << cam.getUp() << std::endl;
        std::cout << "camera viewdir: " << cam.getViewDir() << std::endl;
        std::cout << "mx: " << mx << std::endl;
        std::cout << "my: " << my << std::endl;

        for (size_t i = 0; i < 5; i++) {
            printf("\033[F");
        }

        auto view =
            trans::lookAt(cam.eye, cam.eye + cam.getViewDir(), cam.getUp());

        float fov = 45.0f; // in degree
        /* to specify the near point we specify the distance of near plane from
         * camera we know near plane is always assumed to be in negative z axis
         * so, 0.1f here means z plane lies in z=-0.1f but we specify only
         * distance(i.e positive value) same goes with farPlane
         */
        float nearPlane = 0.1f;
        float farPlane = 100.0f;

        auto per = trans::my_PerspectiveFOV(45.0f, window_width / window_height,
                                            nearPlane, farPlane);
        std::vector<vec4> vertices;
        for (auto &p : cube) {
            auto temp = per * view * scale3d * p;
            vertices.push_back(temp);
            final_cube.emplace_back(
                /* TODO: epsilon wala expression thyo yaa maile hataideko xa
             chainxa vane halnu parne xa */
                /* why fabs for temp.w ?????????????????????????????????????? */

                /* converting normalized devicce coordinate to screen coordinate
                 */
                (int)round(((temp.x / temp.w) + 1) * window_width / 2 -
                           window_width / 2),
                (int)round(((temp.y / temp.w) + 1) * window_height / 2 -
                           window_height / 2));
        }

        for (auto &p : axes_endpoints) {
            auto temp = per * view * scale3d * p;
            final_axes.emplace_back(
                (int)round(temp.x /
                           (fabs(temp.w) < epsilon ? epsilon : temp.w)),
                (int)round(temp.y /
                           (fabs(temp.w) < epsilon ? epsilon : temp.w)));
        }

        vec3_T<float> brescolor(1, 0, 0);

        graphicsEngine->clear();
        // graphicsEngine->drawLines(final_axes);
        // graphicsEngine->drawLines(final_cube, brescolor, cube_indices);
        graphicsEngine->drawTraingles(vertices, cube_indices, cam.viewDir);

        // graphicsEngine.draw_bresenham_adjusted(50, 100, -200, -100, vec3(1,
        // 0, 0));
        graphicsEngine->draw();
        // glClear(GL_COLOR_BUFFER_BIT);
        // glBegin(GL_TRIANGLES);
        // glColor4f(1, 0, 0, 0);
        // glVertex2i(0, 0);
        // glColor4f(0, 1, 0, 1);
        // glVertex2i(0, 200);
        // glColor4f(0, 0, 1, 1);
        // glVertex2i(100, 100);
        // glEnd();
        // glFlush();
        glfwSwapBuffers(window);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    delete graphicsEngine;
    return 0;
}