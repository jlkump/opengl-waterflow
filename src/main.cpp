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

GLFWwindow* window;
const int kWindowWidth = 1024;
const int kWindowHeight = 768;

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

    /* Initialize glad */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return false;
    }

    /* For mouse input */
    // glfwSetMouseButtonCallback(window, MouseButtonCallback);

    /* For Keyboard Input */
    // glfwSetKeyCallback(window, KeyButtonCallback);

    /* Set the viewport */
    glClearColor(0.6784f, 0.8f, 1.0f, 1.0f);
    glViewport(0, 0, kWindowWidth, kWindowHeight);

    glEnable(GL_DEPTH_TEST);

    return true;
}

#define NUM_PARTICLES 500 // This must match the number of particles in the pic_flip_shader.comp

void UpdateLoop() 
{
    float start_time = static_cast<float>(glfwGetTime());
    float previous_time = start_time;
    float new_time = 0.0f;

    struct ComputeData {
        float deltaTime;
        glm::vec3 position[NUM_PARTICLES];
        glm::vec3 velocity[NUM_PARTICLES];
    };
    struct ComputeData c_data;
    c_data.deltaTime = 0;

    ComputeShader compute_shader("waterflow_shader.comp", glm::ivec3(512, 512, 1));
    GLuint ssbo = compute_shader.GenerateAndBindSSBO(&c_data, 4, 0);

    // Use the position of compute data to render water

    /* Loop until the user closes the window */
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window))
    {
        /* Update game time value */
        new_time = static_cast<float>(glfwGetTime());
        c_data.deltaTime = new_time - previous_time;
        previous_time = new_time;

        ////////////////////
        // Compute Shader //
        ////////////////////
        compute_shader.SetActive();
        compute_shader.UpdateSSBO(ssbo, &c_data, 8, 0);

        compute_shader.Dispatch();
        compute_shader.Barrier();

        ////////////////////
        //  3D Rendering  //
        ////////////////////
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
        return -1;
    }

    UpdateLoop();
    glfwTerminate();

	return 0;
}