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

// TODO: Defined a scene renderer class that makes it so we don't need all these global variables
// SceneRenderer g_scene_renderer; // Holds camera, models, skybox, etc.

bool UpdateView(const glm::mat4& view) {
    if (g_skybox)
        g_skybox->SetView(view);
    if (g_debug_renderer)
        g_debug_renderer->SetViewMat(view);
    return true;
}

bool UpdateProjection(const glm::mat4& proj) {
    if (g_skybox)
        g_skybox->SetProjection(proj);
    if (g_debug_renderer)
        g_debug_renderer->SetProjectionMat(proj);
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
    if (g_cam == nullptr) {
        return;
    }

    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS) {
            g_cam->ForwardPressed();
        } else if (action == GLFW_RELEASE) {
            g_cam->ForwardReleased();
        }
    }
    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) {
            g_cam->BackPressed();
        }
        else if (action == GLFW_RELEASE) {
            g_cam->BackReleased();
        }
    }
    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) {
            g_cam->LeftPressed();
        }
        else if (action == GLFW_RELEASE) {
            g_cam->LeftReleased();
        }
    }
    if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) {
            g_cam->RightPressed();
        }
        else if (action == GLFW_RELEASE) {
            g_cam->RightReleased();
        }
    }
    if (key == GLFW_KEY_I) {
        if (action == GLFW_PRESS) {
            g_cam->UpPressed();
        }
        else if (action == GLFW_RELEASE) {
            g_cam->UpReleased();
        }
    }
    if (key == GLFW_KEY_K) {
        if (action == GLFW_PRESS) {
            g_cam->DownPressed();
        }
        else if (action == GLFW_RELEASE) {
            g_cam->DownReleased();
        }
    }
    UpdateView(g_cam->GetCam()->GetViewMatrix());
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

void InitializeDummyGridVel(Texture3D& tex) {
    glm::ivec3 dim = tex.GetDimensions();
    std::vector<glm::vec4> new_data;
    for (int i = 0; i < dim.x; i++) {
        for (int j = 0; j < dim.y; j++) {
            for (int k = 0; k < dim.z; k++) {
                new_data.push_back(glm::vec4(0, 1, 0, 0));
            }
        }
    }
    tex.ModifyTextureData(glm::ivec3(0, 0, 0), dim, (void*)&new_data[0]);
}

bool LoadContent()
{
    /* Create camera for scene */
    g_cam = new FPSCamera(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, 0.0f, -1.0f));

    /* Create Skybox for scene */
    g_skybox = new Skybox({ "skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg" });
    g_skybox->SetView(g_cam->GetCam()->GetViewMatrix());
    g_skybox->SetProjection(g_cam->GetCam()->GetProjectionMatrix());

    Texture3D temp_grid_vel = Texture3D(glm::ivec3(3, 3, 3));
    InitializeDummyGridVel(temp_grid_vel);

    g_debug_renderer = new DebugRenderer();
    g_debug_renderer->SetGridBoundaries(glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0.5, 0.5, 0.5));
    g_debug_renderer->SetGridCellInterval(0.5);
    // g_debug_renderer->SetGridVelocities(temp_grid_vel);
    g_debug_renderer->SetViewMat(g_cam->GetCam()->GetViewMatrix());
    g_debug_renderer->SetProjectionMat(g_cam->GetCam()->GetProjectionMatrix());

    return true;
}

void UpdateLoop() 
{
    float previous_time = static_cast<float>(glfwGetTime());
    float new_time = 0.0f;
    float last_time_updated = 0.0f;
    float time_step = 1.0f;
    
    DisplayText frame_time_display = DisplayText("0.0 ms/frame");
    DebugRenderer debug = DebugRenderer();
    Texture3D temp_grid_vel = Texture3D(glm::ivec3(4, 4, 4));
    InitializeDummyGridVel(temp_grid_vel);

    debug.SetViewMat(g_camera->GetViewMatrix());
    debug.SetProjectionMat(g_camera->GetProjectionMatrix());
    printf("Camera view direction is (%f %f %f)\n", g_camera->GetForward().x, g_camera->GetForward().y, g_camera->GetForward().z);
    debug.SetGridBoundaries(glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0.5, 0.5, 0.5));
    debug.SetGridCellInterval(0.5);
    debug.SetGridVelocities(temp_grid_vel);
    /* Loop until the user closes the window or presses ESC */
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) {
            // printf("%f ms/frame\n", 1000.0 / double(nbFrames));
            frame_time_display.SetText(std::to_string(1000.0 / double(nbFrames)) + " ms/frame");
            nbFrames = 0;
            lastTime += 1.0;
        }
        /* Update game time value */
        new_time = static_cast<float>(glfwGetTime());
        float deltaTime = new_time - previous_time;
        previous_time = new_time;
        g_cam->Process(deltaTime);

        if (new_time - last_time_updated >= time_step && g_simulate) {
            // Perform new step in simulation
            last_time_updated = new_time;
        }

        ////////////////////
        //  3D Rendering  //
        ////////////////////
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        // g_skybox->Draw();
        frame_time_display.Draw();
        g_debug_renderer->Draw();


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