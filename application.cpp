#include <iostream>
#include <vector>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "transformations.h"
#include "camera.h"
#include "engine.h"

mat4 translate3d;
mat4 scale3d;
camera cam;

const std::vector<vec4> cube = {{0, 0, 0, 1}, {0, 0, 50, 1}, {0, 50, 50, 1}, {0, 50, 0, 1}, {50, 0, 0, 1}, {50, 0, 50, 1}, {50, 50, 50, 1}, {50, 50, 0, 1}};

const std::vector<uint32_t> cube_indices = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};

const std::vector<vec4> axes_endpoints = {
    {-100, 0, 0, 1},
    {100, 0, 0, 1},
    {0, 100, 0, 1},
    {0, -100, 0, 1},
    {0, 0, 100, 1},
    {0, 0, -100, 1}};

float zvp = 50, zprp = 200;

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW Error: %s\n", description);
}

static bool captured = false;
static int mx, my;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    auto dir = cam.center - cam.eye;
    // const float cameraSpeed = 2.5 * s.deltatime;
    vec3 left = vec3::cross(cam.up, dir);
    switch (action)
    {
    case GLFW_PRESS:
        switch (key)
        {
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

void processHoldEvent(GLFWwindow *window)
{
    auto dir = cam.center - cam.eye;

    // const float cameraSpeed = 2.5 * s.deltatime;
    vec3 left = vec3::cross(cam.up, dir);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cam.eye += dir * cam.speed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cam.eye -= vec3::normalize(vec3::cross(dir, cam.up)) * cam.speed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cam.eye -= dir * cam.speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cam.eye += vec3::normalize(vec3::cross(dir, cam.up)) * cam.speed;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        cam.eye += cam.up * cam.speed;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        cam.eye -= cam.up * cam.speed;
    }
}

static void cursor_position_callback(GLFWwindow *window, double x, double y)
{
    // printf("(x=%d, y=%d)\n", x, y);
    if (captured)
    {
        float xoffset = mx - x;
        float yoffset = my - y;
        mx = x;
        my = y;

        float sensitivity = 0.09;
        xoffset *= cam.sensitivity;
        yoffset *= cam.sensitivity;

        cam.DelYaw(xoffset);
        cam.DelPitch(yoffset);
    }
}

int window_width, window_height;
engine *graphicsEngine;

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // auto oldWidth = window_width;
    // auto oldHeight = window_height;

    window_height = height;
    window_width = width;

    graphicsEngine->x_size = window_width;
    graphicsEngine->y_size = window_height;
    delete[] graphicsEngine->grid;
    delete[] graphicsEngine->color;
    graphicsEngine->grid = new bool[window_height * window_width];
    graphicsEngine->color = new vec3[window_height * window_width];
    graphicsEngine->clear();

    glViewport(0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, window_width, 0.0, window_height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv)
{

    GLFWwindow *window;

    if (!glfwInit())
        return -1;
    glfwSetErrorCallback(error_callback);

    window = glfwCreateWindow(640, 480, "main", NULL, NULL);
    if (!(window))
    {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
    }

    glfwSwapInterval(1);

    glfwGetFramebufferSize(window, &window_width, &window_height);
    graphicsEngine = new engine(window_width, window_height);

    float rotation_angle = 0;
    float view_angle = 0.0;
    float angle_rotated = 0;
    float zvp = 50, zprp = 200;
    vec3 eye(1, 0.5, 1);
    vec3 center(0, 0, 0);

    while (!glfwWindowShouldClose(window))
    {
        rotation_angle += 1;
        view_angle += 0.1;
        if (rotation_angle > 360)
        {
            rotation_angle = 0;
        }
        if (view_angle > 180)
        {
            view_angle = 1;
        }
        // zvp -= 0.5;

        translate3d = translation(vec3(-25, -25, -25));
        scale3d = scaling3d(vec3(2));

        std::vector<vec2_T<int>> final_cube, final_axes;

        vec3 neweye = y_rotation(1) * vec4(eye);
        angle_rotated++;

        vec3 viewdir(center - eye);
        vec3 up = vec3::cross(neweye - eye, viewdir).normalize();
        eye = neweye;

        auto view = lookAt(eye, center, up);
        auto per = perspectivez(zprp, zvp);
        auto ob = oblique_projection(90, view_angle);

        // const vec3 z = (eye - center).normalize();
        // const vec3 y = vec3::normalize(up);
        // const vec3 x = vec3::cross(y, z).normalize();
        // const vec3 c = -center;

        std::vector<vec4> temp_cube;
        for (auto &p : cube)
        {
            // auto temp = translate3d * p;
            // temp = translation(-eye) * temp;
            // temp = rot * temp;
            auto temp = per * view * scale3d * translate3d * p;
            final_cube.emplace_back((int)round(temp.x / (fabs(temp.w) < epsilon ? epsilon : temp.w)), (int)round(temp.y / (fabs(temp.w) < epsilon ? epsilon : temp.w)));
        }

        // temp_cube = oblique(temp_cube, 10, 45, 60);
        // temp_cube = viewTrans(temp_cube, {0, 0, 1});
        // temp_cube = perspective(temp_cube, 0, {0, 0, 150});

        for (auto &p : axes_endpoints)

        {
            // auto temp = rot * translation(-eye) * p;
            auto temp = per * view * scale3d * p;
            final_axes.emplace_back((int)round(temp.x / (fabs(temp.w) < epsilon ? epsilon : temp.w)), (int)round(temp.y / (fabs(temp.w) < epsilon ? epsilon : temp.w)));
        }

        vec3_T<float> brescolor(1, 0, 0);

        graphicsEngine->clear();
        graphicsEngine->drawLines(final_axes);
        graphicsEngine->drawLines(final_cube, brescolor, cube_indices);
        // graphicsEngine.draw_bresenham_adjusted(50, 100, -200, -100, vec3(1, 0, 0));
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
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    delete graphicsEngine;
    return 0;
}