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
#include "rendering/texture.hpp"
#include "rendering/shader.hpp"
#include "rendering/compute_shader.hpp"
#include "rendering/model.h"
#include "rendering/camera.hpp"
#include "simulation/water_particle_renderer.hpp"

GLFWwindow* window;
const int kWindowWidth = 1024;
const int kWindowHeight = 768;

Model* g_model = nullptr;
Shader* g_shader_vel = nullptr;
Shader* g_shader_dye = nullptr;
Camera* g_camera = nullptr;
bool g_simulate = false;

Skybox* g_skybox = nullptr;

// TODO: Defined a scene renderer class that makes it so we don't need all these global variables
// SceneRenderer g_scene_renderer; // Holds camera, models, skybox, etc.

glm::mat4 model_matrix = glm::mat4(1.0f);

void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    if (g_camera != nullptr)
    {
        // TODO: Move this to a scene renderer where we just call a method
        // g_scene_renderer->SetAspectRatio()
        g_camera->SetAspectRatio(width, height);
        if (g_shader_vel != nullptr)
            g_shader_vel->SetUniformMatrix4fv("proj_view", g_camera->GetProjectionMatrix() * g_camera->GetViewMatrix());

        if (g_shader_dye != nullptr)
            g_shader_dye->SetUniformMatrix4fv("proj_view", g_camera->GetProjectionMatrix() * g_camera->GetViewMatrix());
        if (g_skybox != nullptr) {
            g_skybox->SetProjection(g_camera->GetProjectionMatrix());
            g_skybox->SetView(g_camera->GetViewMatrix());
        }
    }
}

void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // TODO: Use scene renderer to get camera and manipulate for debugging
    // g_scene_renderer->GetActiveCamera();
    // Then manipulate the camera based on key input
    // After done, call g_scene_renderer->UpdateActiveCamera(cam); to modify the camera data
    // Possibly have g_scene_renderer->SetActiveCamera(); to allow for multiple camera views?
    
    // Model rotation
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        model_matrix = glm::rotate(model_matrix, glm::radians(-45.0f), glm::vec3(0, 1, 0));
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        model_matrix = glm::rotate(model_matrix, glm::radians(45.0f), glm::vec3(0, 1, 0));

    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        g_simulate = !g_simulate;
}

bool Init() {
    /* Initialize the library */
    if (!glfwInit())
        return false;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // Resizing will make rendering to texture difficult

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
    glfwSetKeyCallback(window, WindowKeyCallback);

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

bool LoadContent()
{
    /* Create camera for scene */
    g_camera = new Camera(glm::vec3(0.5f, 1.5f, 2.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    g_shader_vel = new Shader("viz_velocity_grid.vert", "viz_velocity_grid.frag");
    g_shader_dye = new Shader("viz_dye_grid.vert", "viz_dye_grid.frag");
    g_skybox = new Skybox({ "skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg" });
    g_skybox->SetProjection(g_camera->GetProjectionMatrix());
    g_skybox->SetView(g_camera->GetViewMatrix());
    return true;
}

void UpdateLoop() 
{
    float previous_time = static_cast<float>(glfwGetTime());
    float new_time = 0.0f;
    float last_time_updated = 0.0f;
    float time_step = 1.0f;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    
    /* Loop until the user closes the window or presses ESC */
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) {
            printf("%f ms/frame\n", 1000.0 / double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }
        /* Update game time value */
        new_time = static_cast<float>(glfwGetTime());
        float deltaTime = new_time - previous_time;
        previous_time = new_time;

        if (new_time - last_time_updated >= time_step && g_simulate) {
            // Perform new step in simulation
            last_time_updated = new_time;
        }

        ////////////////////
        //  3D Rendering  //
        ////////////////////
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        g_skybox->Draw();

        // g_shader->SetActive();
        // g_model->Draw();


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

    if (!LoadContent()) {
        fprintf(stderr, "Failure in loading content.");
        return 1;
    }
    UpdateLoop();
    glfwTerminate();

    delete g_shader_vel;
    delete g_shader_dye;
    delete g_model;
    delete g_camera;

	return 0;
}