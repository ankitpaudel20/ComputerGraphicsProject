cmake_minimum_required(VERSION 3.16)
project(opengl_core)

set(CMAKE_CXX_STANDARD 17)

add_executable(Application Application/application.cpp Application/vendor/tinyobj/tiny_obj_loader.cc)
target_include_directories(Application PRIVATE Application/vendor/tinyobj Application/vendor Application/include)


if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
target_link_options(Application PRIVATE PROFILE)
    find_package(GLEW REQUIRED)
    target_link_libraries(Application PRIVATE GLEW::GLEW)
    find_package(glfw3 CONFIG REQUIRED)
    target_link_libraries(Application PRIVATE glfw)
    message("Linked to windows using MSVC")
else()
    if (CMAKE_SYSTEM_NAME STREQUAL Linux)          
        target_link_libraries(Application PRIVATE GLEW glfw GL GLU)
        message("Linked to Linux")

    elseif (CMAKE_SYSTEM_NAME STREQUAL Windows)       
        target_link_libraries(Application PRIVATE freeglut opengl32 glu32 glew32 glfw3)
        message("Linked to windows")
    endif ()
endif()
