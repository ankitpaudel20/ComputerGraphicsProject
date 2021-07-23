#include <chrono>
#include <iostream>
#include <vector>
#include "callbacks.h"
#include "camera.h"
#include "model.h"
#include <chrono>

#include <filesystem>

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

// const std::vector<vec4> cube = {{1.000000, 1.000000, -1.000000, 1.0},
//                                 {1.000000, -1.000000, -1.000000, 1.0},
//                                 {1.000000, 1.000000, 1.000000, 1.},
//                                 {1.000000, -1.000000, 1.000000, 1.0},
//                                 {-1.000000, 1.000000, -1.000000, 1.0},
//                                 {-1.000000, -1.000000, -1.00000, 1.0},
//                                 {-1.000000, 1.000000, 1.000000, 1.0},
//                                 {-1.000000, -1.000000, 1.000000, 1.0}};

// const std::vector<uint32_t> cube_indices = {5, 3, 1, 3, 8, 4, 7, 6, 8, 2, 8,
// 6,
//                                             1, 4, 2, 5, 2, 6, 5, 7, 3, 3, 7,
//                                             8, 7, 5, 6, 2, 4, 8, 1, 3, 4, 5,
//                                             1, 2};

static void error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
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
    float rotation_angle = 0;
    auto lastframe = std::chrono::high_resolution_clock::now();

    auto cube = Model::loadModel_obj(path + "/cube.obj", "cyborg");
    std::vector<vec4> vertices;
    std::vector<uint32_t> indices;
    std::vector<vec4> normals;
    for (int i = 0; i < cube->meshes[0]->m_indices.size(); i++) {
        indices.push_back(cube->meshes[0]->m_indices[i]);
    }
    for (int i = 0; i < cube->meshes[0]->m_vertices.size(); i++) {
        auto vertex = cube->meshes[0]->m_vertices[i].position;
        vertices.push_back(vec4(vertex.x, vertex.y, vertex.z, 1));
    }
    for (int i = 0; i < cube->meshes[0]->m_vertices.size(); i++) {
        auto normal = cube->meshes[0]->m_vertices[i].normal;
        normals.push_back(vec4(normal.x, normal.y, normal.z, 1));
    }

    mat4 mvp_matrix;
    mat4 mvp_matrix2;
    float fov = 45.0f; // in degree
    /* to specify the near point we specify the distance of near plane from
     * camera we know near plane is always assumed to be in negative z axis
     * so, 0.1f here means z plane lies in z=-0.1f but we specify only
     * distance(i.e positive value) same goes with farPlane
     */
    float nearPlane = 0.1f;
    float farPlane = 10.0f;
    while (!glfwWindowShouldClose(window)) {

        deltatime = std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::high_resolution_clock::now() - lastframe)
                        .count();
        std::cout << "FPS:" << 1000000 / deltatime << std::endl;
        lastframe = std::chrono::high_resolution_clock::now();

        glfwPollEvents();
        processHoldEvent(window);

        if (!pause) {
            rotation_angle += 1;
        }

        if (rotation_angle > 360) {
            rotation_angle = 0;
        }
        // translate3d = trans::translation(vec3(-1, -1, -1));

        auto translate = trans::translation(vec3(3, 0, 0));

        mvp_matrix = trans::scaling3d(vec3(0.3, 0.3, 0.3)) *
                     trans::y_rotation(rotation_angle);
        // auto t1 = trans::translation(vec3(1, 0, 0));
        // auto t2 = trans::translation(vec3(-1.0, 0, 0));
        // auto rotate_y = trans::y_rotation(rotation_angle);
        // auto rotate_x = trans::x_rotation(rotation_angle);

        // std::cout << "camera Eye: " << cam.eye << std::endl;
        // std::cout << "camera up: " << cam.getUp() << std::endl;
        // std::cout << "camera viewdir: " << cam.getViewDir() << std::endl;
        // std::cout << "mx: " << mx << std::endl;
        // std::cout << "my: " << my << std::endl;
        // auto view =
        //     trans::lookAt(cam.eye, cam.eye + cam.getViewDir(), cam.getUp());
        mvp_matrix =
            trans::lookAt(cam.eye, cam.eye + cam.getViewDir(), cam.getUp()) *
            mvp_matrix;

        auto per = trans::my_PerspectiveFOV(45.0f, window_width / window_height,
                                            nearPlane, farPlane);
        mvp_matrix =
            trans::my_PerspectiveFOV(cam.FOV, window_width / window_height,
                                     nearPlane, farPlane) *
            mvp_matrix;
        std::vector<vec4> vertices1;
        std::vector<vec4> vertices2;
        for (auto &p : vertices) {
            // auto temp = per * view * scale3d * p;
            auto temp = mvp_matrix * p;
            // auto temp2 = mvp_matrix * translate * p;
            // auto temp2 = per * view * scale3d * p;
            vertices1.push_back(temp);
            // vertices2.push_back(temp2);
            // vertices2.push_back(temp2);
        }
        graphicsEngine->clear();
        // graphicsEngine->rasterize(vertices, cube_indices);
        if (performRasterization) {
            graphicsEngine->rasterize(vertices1, indices, normals);
            // graphicsEngine->rasterize(vertices2, indices);

        } else {
            graphicsEngine->drawTraingles(vertices1, indices);
            // graphicsEngine->drawTraingles(vertices2, indices);
        }
        // graphicsEngine->rasterize(vertices2, indices);
        graphicsEngine->draw();
        glfwSwapBuffers(window);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    delete graphicsEngine;
    return 0;
}