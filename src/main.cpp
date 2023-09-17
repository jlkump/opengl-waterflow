/*
* OpenGL Water Flow simulation project
* Made by Landon Kump
* 
* Resources and Refrences listed in README.md
* Main code for boilerplate OpenGL CMake and window setup comes from
* Tomasz Ga³aj's OpenGLSampleCMake, found here: 
* https://shot511.github.io/2018-05-29-how-to-setup-opengl-project-with-cmake/
* 
* Github link: https://github.com/jlkump/opengl-waterflow
*/

// Graphics Imports
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

// Std Library Imports
#include <iostream>
#include <stdlib.h>
#include <vector>

// Project Imports
#include <RootDir.h>
#include "FluidCube.hpp"
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"
#include "rendering/compute_shader.hpp"
#include "rendering/model.h"

GLFWwindow* window;
const int kWindowWidth = 1024;
const int kWindowHeight = 768;

Model* g_model = nullptr;
Shader* g_shader = nullptr;
Texture* g_texture = nullptr;

/* Matrices */
glm::vec3 cam_position = glm::vec3(0.0f, 1.0f, 1.2f);
glm::vec3 cam_look_at = glm::vec3(0.0f, 0.5f, 0.0f);
glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 world_matrix = glm::mat4(1.0f);
glm::mat4 view_matrix = glm::lookAt(cam_position, cam_look_at, cam_up);
glm::mat4 projection_matrix = glm::perspectiveFov(glm::radians(60.0f), float(kWindowWidth), float(kWindowHeight), 0.1f, 10.0f);

#define NUM_PARTICLES 500 // This must match the number of particles in the pic_flip_shader.comp

void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    projection_matrix = glm::perspectiveFov(glm::radians(60.0f), float(width), float(height), 0.1f, 10.0f);

    if (g_shader != nullptr)
    {
        g_shader->setUniformMatrix4fv("viewProj", projection_matrix * view_matrix);
    }
}


bool Init() 
{
    /* Initialize the library */
    if (!glfwInit())
        return false;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(kWindowWidth, kWindowHeight, "Water Flow Simulation", nullptr, nullptr);

    if (!window)
    {
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Fixing fov and gl_viewport on window re-size */
    glfwSetWindowSizeCallback(window, WindowSizeCallback);

    /* For mouse input */
    // glfwSetMouseButtonCallback(window, MouseButtonCallback);

    /* For Keyboard Input */
    // glfwSetKeyCallback(window, KeyButtonCallback);

    /* Initialize glad */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return false;
    }

    /* Set the viewport */
    glClearColor(0.6784f, 0.8f, 1.0f, 1.0f);
    glViewport(0, 0, kWindowWidth, kWindowHeight);

    glEnable(GL_DEPTH_TEST);

    return true;
}

bool loadContent()
{

    /* Create and apply basic shader */
    g_shader = new Shader("basic.vert", "basic.frag");
    g_shader->setUniformMatrix4fv("model", world_matrix);
    g_shader->setUniformMatrix3fv("norm_matrix", glm::inverse(glm::transpose(glm::mat3(world_matrix))));
    g_shader->setUniformMatrix4fv("proj_view", projection_matrix * view_matrix);

    g_shader->setUniform3fv("ws_cam_pos", cam_position);

    g_texture = new Texture("alliance.png");
    g_texture->ActiveBind();

    g_model = new Model("resources/models/alliance.obj");

    return true;
}

void UpdateLoop() 
{
    float start_time = static_cast<float>(glfwGetTime());
    float previous_time = start_time;
    float new_time = 0.0f;

    //struct ComputeData {
    //    float deltaTime;
    //    glm::vec3 position[NUM_PARTICLES];
    //    glm::vec3 velocity[NUM_PARTICLES];
    //};
    //struct ComputeData c_data;
    //c_data.deltaTime = 0;

    //ComputeShader compute_shader("waterflow_shader.comp", glm::ivec3(512, 512, 1));
    //GLuint ssbo = compute_shader.GenerateAndBindSSBO(&c_data, 4, 0);

    // Use the position of compute data to render water

    /* Loop until the user closes the window or presses ESC */
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window))
    {
        /* Update game time value */
        new_time = static_cast<float>(glfwGetTime());
        //c_data.deltaTime = new_time - previous_time;
        previous_time = new_time;

        ////////////////////
        // Compute Shader //
        ////////////////////
        //compute_shader.SetActive();
        //compute_shader.UpdateSSBO(ssbo, &c_data, 8, 0);

        //compute_shader.Dispatch();
        //compute_shader.Barrier();

        ////////////////////
        //  3D Rendering  //
        ////////////////////
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        g_shader->SetActive();
        g_texture->ActiveBind();
        g_model->Draw();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
}

int main() 
{
    if (!Init()) 
    {
        fprintf(stderr, "Failure in initializing the window.");
        return 1;
    }

    if (!loadContent()) {
        fprintf(stderr, "Failure in loading content.");
        return 1;
    }
    UpdateLoop();
    glfwTerminate();

    delete g_shader;
    delete g_texture;
    delete g_model;

	return 0;
}