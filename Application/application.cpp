#include <iostream>
#include <vector>
#include <chrono>

#include "core.h"
#include "transformations.h"
#include "camera.h"
#include "engine.h"
#include "model.h"



mat4 translate3d;
mat4 scale3d;
camera cam1;
uint32_t deltatime;

const std::vector<vec4> cube = {{0, 0, 0, 1}, {0, 0, 50, 1}, {0, 50, 50, 1}, {0, 50, 0, 1}, {50, 0, 0, 1}, {50, 0, 50, 1}, {50, 50, 50, 1}, {50, 50, 0, 1}};
const std::vector<Vertex> square = {Vertex(vec3(0, 0, 0), vec3(0, 0, 1)), Vertex(vec3(5, 0, 0), vec3(0, 0, 1)), Vertex(vec3(5, 5, 0), vec3(0, 0, 1)), Vertex(vec3(0, 5, 0), vec3(0, 0, 1))};
const std::vector<uint32_t> square_indices = {0, 1, 2, 0, 2, 3};

const std::vector<uint32_t> cube_indices = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
const std::vector<uint32_t> cube_indices_triangles = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

const std::vector<vec4> axes_endpoints = {
    {-100, 0, 0, 1},
    {100, 0, 0, 1},
    {0, 100, 0, 1},
    {0, -100, 0, 1},
    {0, 0, 100, 1},
    {0, 0, -100, 1}};

float zvp = 50, zprp = 200;

static void error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
}

static bool captured = false;
static int mx, my;
engine *graphicsEngine;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // auto dir = cam1.center - cam1.eye;
    // const float cameraSpeed = 2.5 * s.deltatime;
    vec3 left = vec3::cross(cam1.getUp(), cam1.getViewDir());
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

void processHoldEvent(GLFWwindow *window) {
    // auto dir = cam1.center - cam1.eye;

    const float cameraSpeed = cam1.speed * deltatime;
    vec3 left = vec3::normalize(vec3::cross(cam1.getUp(), cam1.getViewDir()));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cam1.eye += (cam1.getViewDir() * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cam1.eye += (left * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cam1.eye -= (cam1.getViewDir() * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cam1.eye -= (left * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        cam1.eye += (cam1.getUp() * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        cam1.eye -= (cam1.getUp() * cameraSpeed);
    }

    float factor = 1;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        cam1.DelPitch(cam1.sensitivity * deltatime);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        cam1.DelPitch(-cam1.sensitivity * deltatime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        cam1.newDelYaw(-cam1.sensitivity * deltatime * factor);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        cam1.newDelYaw(cam1.sensitivity * deltatime * factor);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS) {
        graphicsEngine->nearPlane -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS) {
        graphicsEngine->nearPlane += 0.1;
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
        xoffset *= cam1.sensitivity * deltatime;
        yoffset *= cam1.sensitivity * deltatime;

        cam1.newDelYaw(-xoffset);
        cam1.DelPitch(-yoffset);
    }
}

int window_width, window_height;

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
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

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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

    auto path = searchRes();

    auto model = Model::loadModel_obj(path + "/color/testcolored.obj", "arrow");

    float rotation_angle = 0;
    float view_angle = 0.0;
    float angle_rotated = 0;
    float zvp = 50, zprp = -500;

    cam1.eye = vec3(-4.11877, 3.28811, 2.87941);
    cam1.changeDir(vec3(0.56935, -0.659947, -0.490215));
    graphicsEngine->nearPlane = 2;
    Material m;
    m.diffuseColor = color(255, 0, 0);
    graphicsEngine->currentMaterial = &m;
    graphicsEngine->cullBackface = true;
    graphicsEngine->cam =& cam1;
    graphicsEngine->dirlight = dirLight(vec3(-1, -1, -1).normalize(), 2, color(255));
    

    auto lastframe = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
        deltatime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - lastframe).count();
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

        translate3d = trans::translation(vec3(5, 0, -5));
        scale3d = trans::scaling3d(vec3(2));

        std::vector<vec2_T<int>> final_cube, final_axes;

        std::cout << "FPS: " << 1e6 / deltatime << std::endl;
        std::cout << "camera Eye: " << cam1.eye << std::endl;
        std::cout << "camera up: " << cam1.getUp() << std::endl;
        std::cout << "camera viewdir: " << cam1.getViewDir() << std::endl;
        

        for (size_t i = 0; i < 4; i++) {
            printf("\033[F");
        }

        //printingtime = std::chrono::high_resolution_clock::now();

        // auto view = trans::lookAt(cam1.eye, cam1.eye + cam1.getViewDir(), cam1.getUp());
        // // auto per = trans::perspective(0, 0, zprp, zvp);
        // auto per = trans::persp(window_width, window_height, cam1.FOV);
        // auto ob = trans::oblique_projection(90, view_angle);

        // std::vector<vec4> temp_cube;
        // for (auto &p : cube)
        // {
        //     auto temp = view * scale3d * translate3d * p;
        //     temp = per * temp;
        //     final_cube.emplace_back((int)round(temp.x / (fabs(temp.w) < epsilon ? epsilon : temp.w)), (int)round(temp.y / (fabs(temp.w) < epsilon ? epsilon : temp.w)));
        // }

        // for (auto &p : axes_endpoints)
        // {
        //     auto temp = per * view * scale3d * p;
        //     final_axes.emplace_back((int)round(temp.x / (fabs(temp.w) < epsilon ? epsilon : temp.w)), (int)round(temp.y / (fabs(temp.w) < epsilon ? epsilon : temp.w)));
        // }

        color brescolor(0, 255, 0);

        graphicsEngine->clear();
        // graphicsEngine->drawLines(final_axes);
        // graphicsEngine->drawLines(final_cube, brescolor, cube_indices);
        // graphicsEngine->drawTriangles(cube->meshes[0]->m_vertices, cube->meshes[0]->m_indices, cam1, translate3d);
        // graphicsEngine->drawTrianglesRasterized(cube->meshes[0]->m_vertices, cube->meshes[0]->m_indices, cam1, mat4());

        //m.diffuseColor = color(0, 255, 0);
        for (auto & mesh:model->meshes) {
            graphicsEngine->currentMaterial = &mesh->material;
            graphicsEngine->drawTrianglesRasterized(mesh->m_vertices, mesh->m_indices, cam1, mat4());
            //graphicsEngine->drawTriangles(mesh->m_vertices, mesh->m_indices, cam1, mat4());
        }      
        //graphicsEngine->currentMaterial = &m;
        //graphicsEngine->drawTrianglesRasterized(square, square_indices, cam1, translate3d);
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
    }
    delete graphicsEngine;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}