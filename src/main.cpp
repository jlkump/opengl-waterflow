/*
* OpenGL Water Flow simulation project
* Made by Landon Kump
* 
* Resources and Refrences listed in README.md
* Main code for boilerplate OpenGL CMake and window setup comes from
* Tomasz Ga�aj's OpenGLSampleCMake, found here: 
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
#include "rendering/skybox.hpp"
#include "rendering/display_text.hpp"
#include "simulation/debug_renderer.hpp"
#include "rendering/fps_camera.hpp"

GLFWwindow* window;
const int kWindowWidth = 1024;
const int kWindowHeight = 768;

FPSCamera* g_cam = nullptr;
Skybox* g_skybox = nullptr;
DebugRenderer* g_debug_renderer = nullptr;
bool g_simulate = false;
std::set<int> g_keys_pressed;


// TODO: Defined a scene renderer class that makes it so we don't need all these global variables
// SceneRenderer g_scene_renderer; // Holds camera, models, skybox, etc.

bool UpdateView(const glm::mat4& view) {
    if (g_skybox)
        g_skybox->SetView(view);
    if (g_debug_renderer)
        g_debug_renderer->SetView(view);
    return true;
}

bool UpdateProjection(const glm::mat4& proj) {
    if (g_skybox)
        g_skybox->SetProjection(proj);
    if (g_debug_renderer)
        g_debug_renderer->SetProjection(proj);
    return true;
}

void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    if (g_cam != nullptr) {
        g_cam->SetAspectRatio(width, height);
        UpdateProjection(g_cam->GetCam()->GetProjectionMatrix());
    }

}

void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (g_cam == nullptr || g_debug_renderer == nullptr) {
        return;
    }

    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS) {
            g_keys_pressed.insert(GLFW_KEY_W);
            g_cam->ForwardPressed();
        }
        else if (action == GLFW_RELEASE) {
            g_keys_pressed.erase(GLFW_KEY_W);
            g_cam->ForwardReleased();
        }
    }
    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) {
            g_keys_pressed.insert(GLFW_KEY_S);
            g_cam->BackPressed();
        }
        else if (action == GLFW_RELEASE) {
            g_keys_pressed.erase(GLFW_KEY_S);
            g_cam->BackReleased();
        }
    }
    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) {
            g_keys_pressed.insert(GLFW_KEY_A);
            g_cam->LeftPressed();
        }
        else if (action == GLFW_RELEASE) {
            g_keys_pressed.erase(GLFW_KEY_A);
            g_cam->LeftReleased();
        }
    }
    if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) {
            g_keys_pressed.insert(GLFW_KEY_D);
            g_cam->RightPressed();
        }
        else if (action == GLFW_RELEASE) {
            g_keys_pressed.erase(GLFW_KEY_D);
            g_cam->RightReleased();
        }
    }
    if (key == GLFW_KEY_SPACE) {
        if (action == GLFW_PRESS) {
            g_keys_pressed.insert(GLFW_KEY_SPACE);
            g_cam->UpPressed();
        }
        else if (action == GLFW_RELEASE) {
            g_keys_pressed.erase(GLFW_KEY_SPACE);
            g_cam->UpReleased();
        }
    }
    if (key == GLFW_KEY_LEFT_SHIFT) {
        if (action == GLFW_PRESS) {
            g_keys_pressed.insert(GLFW_KEY_LEFT_SHIFT);
            g_cam->DownPressed();
        }
        else if (action == GLFW_RELEASE) {
            g_keys_pressed.erase(GLFW_KEY_LEFT_SHIFT);
            g_cam->DownReleased();
        }
    }

    // Debug Keys
    if (key == GLFW_KEY_T) {
        if (action == GLFW_PRESS) {
            g_debug_renderer->ToggleDebugView(DebugRenderer::FRAME_TIME);
        }
    }
    if (key == GLFW_KEY_G) {
        if (action == GLFW_PRESS) {
            g_debug_renderer->ToggleDebugView(DebugRenderer::GRID);
        }
    }
    if (key == GLFW_KEY_V) {
        if (action == GLFW_PRESS) {
            g_debug_renderer->ToggleDebugView(DebugRenderer::GRID_VELOCITIES);
        }
    }
    if (key == GLFW_KEY_O) {
        if (action == GLFW_PRESS) {
            g_debug_renderer->ToggleDebugView(DebugRenderer::ORIGIN);
        }
    }
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
    glDepthFunc(GL_LESS);

    return true;
}

bool LoadContent()
{
    /* Create camera for scene */
    g_cam = new FPSCamera(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, 0.0f, -1.0f));

    /* Create Skybox for scene */
    g_skybox = new Skybox({ "skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg" });

    g_debug_renderer = new DebugRenderer();
    g_debug_renderer->SetGridBoundaries(glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0.5, 0.5, 0.5), 0.5f);
    std::vector<glm::vec3> velocities = {
        glm::vec3(0.2f, 0.2f, 0.2f),
        glm::vec3(0.3f, 0.3f, 0.3f),
        glm::vec3(0.4f, 0.4f, 0.4f), // Imaginary
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.6f, 0.6f, 0.6f),
        glm::vec3(0.7f, 0.7f, 0.7f), // Imaginary
        glm::vec3(0.8f, 0.8f, 0.8f), // Imaginary
        glm::vec3(0.9f, 0.9f, 0.9f), // Imaginary
        glm::vec3(20.2f, 20.2f, 20.2f)  // Imaginary
    };
    g_debug_renderer->SetGridVelocities(velocities, 2);


    UpdateView(g_cam->GetCam()->GetViewMatrix());
    UpdateProjection(g_cam->GetCam()->GetProjectionMatrix());

    return true;
}

void UpdateLoop() 
{
    float previous_time = static_cast<float>(glfwGetTime());
    float new_time = 0.0f;
    float last_time_updated = 0.0f;
    float time_step = 1.0f;
    

    /* Loop until the user closes the window or presses ESC */
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) {
            // printf("%f ms/frame\n", 1000.0 / double(nbFrames));
            if (g_debug_renderer != nullptr) {
                g_debug_renderer->UpdateFrameTime(1000.0 / double(nbFrames));
            }
            nbFrames = 0;
            lastTime += 1.0;
        }
        /* Update game time value */
        new_time = static_cast<float>(glfwGetTime());
        float deltaTime = new_time - previous_time;
        previous_time = new_time;
        g_cam->Process(deltaTime);
        if (true) { //g_keys_pressed.size() != 0) { // Very inefficent, but just for testing and sanity check
            UpdateView(g_cam->GetCam()->GetViewMatrix());
            UpdateProjection(g_cam->GetCam()->GetProjectionMatrix());
        }
        if (new_time - last_time_updated >= time_step && g_simulate) {
            // Perform new step in simulation
            last_time_updated = new_time;
        }

        ////////////////////
        //  3D Rendering  //
        ////////////////////
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        g_debug_renderer->Draw();
        // g_skybox->Draw();


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

    delete g_cam;
    delete g_debug_renderer;
    delete g_skybox;

	return 0;
}