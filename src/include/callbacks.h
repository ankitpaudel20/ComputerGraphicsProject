#pragma once

#include "GL/glew.h"
#include "GL/glu.h"
#include "GLFW/glfw3.h"
#include "camera.h"
#include "engine.h"

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);
void processHoldEvent(GLFWwindow *window);
void cursor_position_callback(GLFWwindow *window, double x, double y);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processHoldEvent(GLFWwindow *window);
camera cam;
bool captured = false;
bool pause = false;
bool performRasterization = false;
int mx, my;
float nearPlane = 0.10f;
float farPlane = 10.0f;
bool showTraingle = false;

uint32_t deltatime;
int window_width, window_height;
engine *graphicsEngine;

float xy_scale = 1;
float z_scale = 1;

extern bool gouraud_test = false;

void key_callback(GLFWwindow *window, int key, int scancode, int action,

                  int mods) {
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
            // glfwSetCursorPos(window, mx, my);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            captured = true;
            break;
        case GLFW_KEY_SPACE:
            pause = !pause;
            break;
        case GLFW_KEY_U:
            performRasterization = !performRasterization;
            break;
        case GLFW_KEY_Y:
            gouraud_test = !gouraud_test;
            break;
        case GLFW_KEY_T:
            showTraingle = !showTraingle;
            break;
        default:
            break;
        }
        break;
    }
}

void processHoldEvent(GLFWwindow *window) {
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
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        nearPlane += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        nearPlane -= 0.01;
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

void cursor_position_callback(GLFWwindow *window, double x, double y) {
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

        cam.newDelYaw(-xoffset);
        cam.DelPitch(-yoffset);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // auto oldWidth = window_width;
    // auto oldHeight = window_height;

    window_height = height;
    window_width = width;

    graphicsEngine->resizeFrameBuffer(width, height);
    glViewport(0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, window_width, 0.0, window_height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}