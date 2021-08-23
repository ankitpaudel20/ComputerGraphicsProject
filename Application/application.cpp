#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

#include "core.h"
#include "transformations.h"
#include "camera.h"
#include "engine.h"
#include "model.h"

mat4f translate3d;
mat4f scale3d;
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
static double mx, my;
engine *graphicsEngine;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // auto dir = cam1.center - cam1.eye;
    // const float cameraSpeed = 2.5 * s.deltatime;
    // vec3 left = vec3::cross(cam1.getUp(), cam1.getViewDir());
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
            glfwGetCursorPos(window, &mx, &my);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            captured = true;
            break;
        case GLFW_KEY_6:
            //graphicsEngine->executesubimage2d();
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
    auto light_speed = 0.3f;

    if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) {
        graphicsEngine->pointLights.back().delpos(cam1.getViewDir() * light_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) {
        graphicsEngine->pointLights.back().delpos(-cam1.getViewDir() * light_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) {
        graphicsEngine->pointLights.back().delpos(left * light_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) {
        graphicsEngine->pointLights.back().delpos(-left * light_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        graphicsEngine->pointLights.back().delpos(-cam1.getUp() * light_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        graphicsEngine->pointLights.back().delpos(cam1.getUp() * light_speed);
    }

    //if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS) {
    //    graphicsEngine->pointLights.back().delpos(cam1.getUp() * light_speed);
    //}

    //if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS) {
    //    graphicsEngine->pointLights.back().delpos(-cam1.getUp() * light_speed);
    //}

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
        if (graphicsEngine->nearPlane < 0.01)
            graphicsEngine->nearPlane = 0.01;
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

        xoffset *= cam1.sensitivity * deltatime;
        yoffset *= cam1.sensitivity * deltatime;
        const float bias = 1.f - fabs(vec3::dot(cam1.getViewDir(), vec3(0, 1, 0)));
        cam1.newDelYaw(-xoffset * bias);
        cam1.DelPitch(-yoffset);
    }
}

int window_width, window_height;

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {

    window_height = height;
    window_width = width;

    delete graphicsEngine->fboCPU;
    graphicsEngine->fboCPU = new framebuffer(width, height);

    glViewport(0, 0, window_width, window_height);

    GLcall(glGenTextures(1, &graphicsEngine->tex));
    GLcall(glActiveTexture(GL_TEXTURE0));
    GLcall(glBindTexture(GL_TEXTURE_2D, graphicsEngine->tex));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, graphicsEngine->fboCPU->x_size, graphicsEngine->fboCPU->y_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
    GLcall(glBindTexture(GL_TEXTURE_2D, 0));
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        auto eng = (engine *)glfwGetWindowUserPointer(window);
        glfwGetCursorPos(window, &mx, &my);
        const size_t index = (eng->fboCPU->y_size - my) * eng->fboCPU->x_size + mx;
        color col = eng->fboCPU->colorlayer[index];
        printf("clicked color: (%d,%d,%d)\n", col.r(), col.g(), col.b());
    }
}

vec3 getCenterOfMass(const Vertex *vertices, const unsigned int size) {
    vec3 center(0);
    for (unsigned int i = 0; i < size; i++) {
        center += vertices[i].position;
    }
    return center / size;
}

int main(int argc, char **argv) {

    auto threads = std::thread::hardware_concurrency();

    GLFWwindow *window;

    if (!glfwInit())
        return -1;
    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(640, 480, "main", NULL, NULL);
    if (!(window)) {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
    }

    glfwSwapInterval(1);

    glfwGetFramebufferSize(window, &window_width, &window_height);
    graphicsEngine = new engine(window_width, window_height);
    glfwSetWindowUserPointer(window, graphicsEngine);

    auto path = searchRes();

    node rootNode;

    auto lightmodel = Model::loadModel_obj(path + "/sphere.obj", "light");
    auto lightmodelCenter = getCenterOfMass(lightmodel->meshes.back()->m_vertices.data(), lightmodel->meshes.back()->m_vertices.size());

    // auto colorCube = Model::loadModel_obj(path + "/color/testColored.obj", "color");
    auto textureBox = Model::loadModel_obj(path + "/Crate/Crate1.obj", "texturebox");
    // auto football_real = Model::loadModel_obj(path + "/Football/Football_LowPoly.obj", "football");
    // auto stupa = Model::loadModel_obj(path + "/swayambhunath/swayambhunath.obj", "city");
    auto city = Model::loadModel_obj(path + "/city/city_with_lamp_new.obj", "city");
    // auto texturedSquare = Model::loadModel_obj(path + "/texturedSquare.obj", "city");
    // rootNode.children["city"] = city;

    cam1.eye = vec3(3.40189, 2.46183, -4.55371);
    cam1.changeDir(vec3(-0.438673, -0.210734, 0.873589));
    graphicsEngine->nearPlane = 1;
    Material m;
    m.diffuseColor = color(255, 0, 0);
    graphicsEngine->cullBackface = false;
    graphicsEngine->cam = &cam1;
    graphicsEngine->dirlight = dirLight(vec3(-1, -1, -1).normalize(), 2, color(255));
    graphicsEngine->ambientLightIntensity = 0;

    for (auto &mesh : city->meshes) {
        printf("meshname %s \n", mesh->name.c_str());
        if (mesh->name.find("lightCube") != std::string::npos) {
            // mesh->draw = false;
            const vec3 center = getCenterOfMass(mesh->m_vertices.data(), mesh->m_vertices.size());
            // const vec3 football_center = getCenterOfMass(textureBox->meshes[0]->m_vertices.data(), textureBox->meshes[0]->m_vertices.size());
            // printf("debug Translate matrix: \n");
            // debugTranslate.print();
            for (auto &vertex : mesh->m_vertices) {
                auto debugTranslate = trans::translate(-center);
                vertex.position = debugTranslate * vec4(vertex.position, 1);
            }
            // mesh->matModel = trans::translate(center);
            graphicsEngine->pointLights.emplace_back(center, 1, mesh);
        }
    }

    // graphicsEngine->pointLights.emplace_back(vec3(0.670566, 1.62085, 0.872629), 3, lightmodel->meshes.back());
    //for (auto &mesh : lightModel) {
    //    mesh->doLightCalculations = false;
    //}
    //graphicsEngine->pointLights.back().meshes = lightModel;
    //graphicsEngine->pointLights.back().setScale(vec3(0.1));

    //graphicsEngine->pointLights.emplace_back(vec3(0.670566, 1.62085, 0.872629), 3);

    //auto spherescale = trans::scaling3d(0.1);
    //for (auto &mesh : lightmodels) {
    //    for (auto &point : mesh->meshes.back()->m_vertices) {
    //        point.position = spherescale * point.position;
    //    }
    //}
    //for (int i = 0; i < graphicsEngine->pointLights.size(); i++) {
    //    graphicsEngine->pointLights[i].setmodel(lightmodels[i]);
    //}

    auto lastframe = std::chrono::high_resolution_clock::now();
    std::string fpsString = "FPS: ";
    const auto place = fpsString.find_first_of(" ") + 1;

    while (!glfwWindowShouldClose(window)) {
        deltatime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - lastframe).count();
        lastframe = std::chrono::high_resolution_clock::now();

        // graphicsEngine->pointLights.back().setpos(graphicsEngine->cam->eye);

        glfwPollEvents();
        processHoldEvent(window);

        translate3d = trans::translate(vec3(5, 0, -5));
        scale3d = trans::scaling3d(vec3(0.5));

        fpsString.replace(fpsString.begin() + place, fpsString.end(), std::to_string(1e6 / deltatime));
        glfwSetWindowTitle(window, fpsString.c_str());

        std::cout << "camera Eye: " << cam1.eye << std::endl;
        std::cout << "camera viewdir: " << cam1.getViewDir() << std::endl;

        printf("\033[F");
        printf("\033[F");

        graphicsEngine->clear();

        //for (auto &mesh : lightmodel->meshes) {
        //    graphicsEngine->currentMesh = mesh;
        //    if (mesh->doLightCalculations && mesh->draw) {
        //        // graphicsEngine->makeRequiredTriangles(mesh->m_vertices, mesh->m_indices, mat4f());
        //    }
        //}

        for (auto &mesh : city->meshes) {
            graphicsEngine->currentMesh = mesh;
            if (mesh->doLightCalculations && mesh->draw) {
                // if (mesh->name.find("lightCube") != std::string::npos)
                graphicsEngine->makeRequiredTriangles(mesh->m_vertices, mesh->m_indices, mesh->matModel);
            }
        }

        //graphicsEngine->currentMesh = textureBox->meshes[0];
        //if (textureBox->meshes[0]->doLightCalculations && textureBox->meshes[0]->draw) {
        //    graphicsEngine->makeRequiredTriangles(textureBox->meshes[0]->m_vertices, textureBox->meshes[0]->m_indices, mat4f());
        //}

        for (auto &light : graphicsEngine->pointLights) {
            // graphicsEngine->makeRequiredTriangles(textureBox->meshes[0]->m_vertices, textureBox->meshes[0]->m_indices, mat4f());
            light.draw(*graphicsEngine);
        }

        //rootNode.draw(*graphicsEngine);
        //graphicsEngine->currentMaterial = &m;
        //graphicsEngine->drawTrianglesRasterized(square, square_indices, translate3d);
        graphicsEngine->draw();
        glfwSwapBuffers(window);
    }
    delete graphicsEngine;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}