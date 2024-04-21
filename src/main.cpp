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
#include "simulation/sequential_simulation.hpp"
#include "simulation/gpu_simulation.hpp"

GLFWwindow* window;
const int kWindowWidth = 1024;
const int kWindowHeight = 768;

FPSCamera* g_cam = nullptr;
WaterParticleRenderer* g_particle_renderer = nullptr;
Skybox* g_skybox = nullptr;
DebugRenderer* g_debug_renderer = nullptr;
Simulation* g_sim = nullptr;
bool g_simulate = false;
bool g_draw_realistic = false;
std::set<int> g_keys_pressed;

enum SimulationType {
    SEQ_GRID,
    SEQ_PARTICLE,
    GPU_PARTICLE
};

const int TOTAL_SIMULATION_TYPES = 3;
SimulationType simulation_type = SimulationType::GPU_PARTICLE;
bool enable_particles = false;

// TODO: Defined a scene renderer class that makes it so we don't need all these global variables
// SceneRenderer g_scene_renderer; // Holds camera, models, skybox, etc.

// Prototypes
void ChangeSimulationType(bool getNext);
void SetSimulation();

bool UpdateView(const glm::mat4& view) {
    if (g_skybox)
        g_skybox->SetView(view);
    if (g_debug_renderer)
        g_debug_renderer->SetView(view);
    if (g_particle_renderer)
        g_particle_renderer->UpdateViewMat(view);
    return true;
}

bool UpdateProjection(const glm::mat4& proj) {
    if (g_skybox)
        g_skybox->SetProjection(proj);
    if (g_debug_renderer)
        g_debug_renderer->SetProjection(proj);
    if (g_particle_renderer)
        g_particle_renderer->UpdateProjMat(proj);
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

    // Camera movement
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
    if (key == GLFW_KEY_B) {
        if (action == GLFW_PRESS) {
            g_debug_renderer->ToggleDebugView(DebugRenderer::GRID_AXIS_VELOCITIES);
        }
    }
    if (key == GLFW_KEY_O) {
        if (action == GLFW_PRESS) {
            g_debug_renderer->ToggleDebugView(DebugRenderer::ORIGIN);
        }
    }
    if (key == GLFW_KEY_N) {
        if (action == GLFW_PRESS) {
            g_debug_renderer->ToggleDebugView(DebugRenderer::PARTICLES);
        }
    }
    if (key == GLFW_KEY_M) {
        if (action == GLFW_PRESS) {
            g_debug_renderer->ToggleDebugView(DebugRenderer::PARTICLE_VELOCITIES);
        }
    }
    if (key == GLFW_KEY_P) {
        if (action == GLFW_PRESS) {
            g_simulate = !g_simulate;
        }
    }

    if (key == GLFW_KEY_COMMA) {
        if (action == GLFW_PRESS) {
            ChangeSimulationType(false);
            SetSimulation();
        }
    }
    if (key == GLFW_KEY_PERIOD) {
        if (action == GLFW_PRESS) {
            ChangeSimulationType(true);
            SetSimulation();
        }
    }

    if (key == GLFW_KEY_I) {
        if (action == GLFW_PRESS) {
            g_draw_realistic = !g_draw_realistic;
        }
    }

    //if (key == GLFW_KEY_1) {
    //    if (action == GLFW_PRESS) {
    //        if (g_sim != nullptr && g_sim->GetGridPressures() != nullptr) {
    //            if (g_debug_renderer->IsCellViewActive(DebugRenderer::PRESSURE) || !g_debug_renderer->IsDebugViewActive(DebugRenderer::GRID_CELL)) {
    //                g_debug_renderer->ToggleDebugView(DebugRenderer::GRID_CELL);
    //            }
    //            g_debug_renderer->SetGridPressures(*g_sim->GetGridPressures(), g_sim->GetGridDimensions());
    //        }
    //    }
    //}
    //if (key == GLFW_KEY_2) {
    //    if (action == GLFW_PRESS) {
    //        if (g_sim != nullptr && g_sim->GetGridDyeDensities() != nullptr) {
    //            if (g_debug_renderer->IsCellViewActive(DebugRenderer::DYE) || !g_debug_renderer->IsDebugViewActive(DebugRenderer::GRID_CELL)) {
    //                g_debug_renderer->ToggleDebugView(DebugRenderer::GRID_CELL);
    //            }
    //            g_debug_renderer->SetGridDyeDensities(*g_sim->GetGridDyeDensities(), g_sim->GetGridDimensions());
    //        }
    //    }
    //}
    //if (key == GLFW_KEY_3) {
    //    if (action == GLFW_PRESS) {
    //        if (g_sim != nullptr && g_sim->GetGridFluidCells() != nullptr) {
    //            printf("Displaying fluid cells\n");
    //            if (g_debug_renderer->IsCellViewActive(DebugRenderer::IS_FLUID) || !g_debug_renderer->IsDebugViewActive(DebugRenderer::GRID_CELL)) {
    //                printf("Toggling cell display\n");
    //                g_debug_renderer->ToggleDebugView(DebugRenderer::GRID_CELL);
    //            }
    //            g_debug_renderer->SetGridFluidCells(*g_sim->GetGridFluidCells(), g_sim->GetGridDimensions());
    //        }
    //    }
    //}
}

void APIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
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
    glEnable(GL_BLEND);
    glDepthFunc(GL_LESS);

    //// During init, enable debug output
    //glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(MessageCallback, 0);

    return true;
}

void ChangeSimulationType(bool getNext) {
    if (getNext) { // Increment
        simulation_type = static_cast<SimulationType>((simulation_type + 1) % TOTAL_SIMULATION_TYPES);
    }
    else { // Decrement
        if (simulation_type - 1 < 0) {
            simulation_type = static_cast<SimulationType>(TOTAL_SIMULATION_TYPES - 1);
        }
        else {
            simulation_type = static_cast<SimulationType>(simulation_type - 1);
        }
    }
}

/**
 * 0 - Sequential grid
 * 1 - Sequential particle
 * 2 - GPU particle
 *
 * TODO: construct and deconstruct debug renderer based on simulation types?
 */
void SetSimulation() {
    if (g_debug_renderer != nullptr) {
        g_debug_renderer->ResetActiveViews();
    }
    delete g_sim;
    g_sim = nullptr;
    g_simulate = false;

    const int num_particles = 4096;
    const float interval = 0.25;
    const glm::vec3 low_bound = glm::vec3(-1.0, -1.0, -1.0);
    const glm::vec3 up_bound = glm::vec3(1.0, 1.0, 1.0);
    const int grid_dim = (up_bound.x - low_bound.x) / interval;
    printf("Grid dim: %d\n", grid_dim);

    switch (simulation_type) {
    case SimulationType::SEQ_GRID:
        printf("Simulation set to (SEQ_GRID)\n");
        g_sim = new SequentialGridBased();
        enable_particles = false;
        break;

    case SimulationType::SEQ_PARTICLE:
        printf("Simulation set to (SEQ_PARTICLE)\n");
        g_sim = new SequentialParticleBased();
        enable_particles = true;
        break;

    case SimulationType::GPU_PARTICLE:
        printf("Simulation set to (GPU_PARTICLE)\n");
        // GPU_Simulation(int num_particles_sqrt, int grid_dim, int iteration)
        g_sim = new GPU_Simulation(static_cast<int>(sqrt(num_particles)), grid_dim, 40);
        break;

    default:
        printf("main.cpp: SetSimulation(): invalid simulation type: %d\n", simulation_type);
        exit(-1);
    }

    // TODO: Setup simulation variables
    std::vector<glm::vec3> init_particle_vel;
    for (int i = 0; i < num_particles; i++) {
        //init_particle_vel.push_back(glm::normalize(glm::vec3(
        //    ((float)(rand() % 100) / 100.0f),
        //    ((float)(rand() % 100) / 100.0f),
        //    ((float)(rand() % 100) / 100.0f)))
        //);
        init_particle_vel.push_back(glm::vec3(0.0f));
    }

    g_sim->SetInitialVelocities(
        init_particle_vel,
        low_bound, 
        up_bound, 
        interval
    );

    // Create/Update debug renderer
    if (g_debug_renderer != nullptr) {
        if (simulation_type != SimulationType::GPU_PARTICLE) {
            g_debug_renderer->SetGridBoundaries(g_sim->GetGridLowerBounds(), g_sim->GetGridUpperBounds(), g_sim->GetGridInterval());
            g_debug_renderer->SetGridVelocities(*g_sim->GetGridVelocities(), g_sim->GetGridDimensions());

            if (simulation_type == SimulationType::SEQ_PARTICLE) {
                g_debug_renderer->SetParticlePositions(*g_sim->GetParticlePositions());
                g_debug_renderer->SetParticleVelocities(*g_sim->GetParticlePositions(), *g_sim->GetParticleVelocities());
            }
        }
    }

}

bool LoadContent()
{
    // Create camera for scene 
    g_cam = new FPSCamera(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, 0.0f, -1.0f));

    // Create Skybox for scene
    g_skybox = new Skybox({ "skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg" });
    g_debug_renderer = new DebugRenderer();

    // Create simulation 
    SetSimulation();

    g_particle_renderer = new WaterParticleRenderer();
    g_particle_renderer->UpdateSkybox(g_skybox);
    g_particle_renderer->UpdateCamera(g_cam->GetCam());
    if (dynamic_cast<GPU_Simulation*>(g_sim) != nullptr)
        g_particle_renderer->UpdateTexturePrecision(dynamic_cast<GPU_Simulation*>(g_sim)->GetTexturePrecision());
    

    UpdateView(g_cam->GetCam()->GetViewMatrix());
    UpdateProjection(g_cam->GetCam()->GetProjectionMatrix());

    return true;
}

void UpdateLoop() 
{
    float previous_time = static_cast<float>(glfwGetTime());
    float new_time = 0.0f;
    float last_time_updated = 0.0f;
    float time_step = 0.1f;

    /* Loop until the user closes the window or presses ESC */
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
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
        if (g_keys_pressed.size() != 0) {
            UpdateView(g_cam->GetCam()->GetViewMatrix());
            UpdateProjection(g_cam->GetCam()->GetProjectionMatrix());
        }
        if (g_simulate && new_time - last_time_updated >= time_step) {
            // Perform new step in simulation
            g_sim->TimeStep(deltaTime + time_step);

            // Update debug renderer
            if (simulation_type != SimulationType::GPU_PARTICLE) {
                g_debug_renderer->SetGridVelocities(*g_sim->GetGridVelocities(), g_sim->GetGridDimensions());

                if (g_sim->GetParticlePositions() != nullptr) {
                    g_debug_renderer->SetParticlePositions(*g_sim->GetParticlePositions());
                    g_debug_renderer->SetParticleVelocities(*g_sim->GetParticlePositions(), *g_sim->GetParticleVelocities());
                }
                if (g_debug_renderer->IsDebugViewActive(DebugRenderer::GRID_CELL)) {
                    switch (g_debug_renderer->GetCellViewActive()) {
                    case DebugRenderer::DYE:
                        g_debug_renderer->SetGridDyeDensities(*g_sim->GetGridDyeDensities(), g_sim->GetGridDimensions());
                        break;
                    case DebugRenderer::IS_FLUID:
                        g_debug_renderer->SetGridDyeDensities(*g_sim->GetGridFluidCells(), g_sim->GetGridDimensions());
                        break;
                    case DebugRenderer::PRESSURE:
                        g_debug_renderer->SetGridPressures(*g_sim->GetGridPressures(), g_sim->GetGridDimensions());
                        break;
                    case DebugRenderer::NONE:
                        break;
                    }
                }
            } else {
                // GPU Simulation rendering
            }

            last_time_updated = new_time;
        }

        ////////////////////
        //  3D Rendering  //
        ////////////////////
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        if (simulation_type == SimulationType::GPU_PARTICLE && dynamic_cast<GPU_Simulation*>(g_sim) != nullptr 
            && dynamic_cast<GPU_Simulation*>(g_sim)->GetTexParticlePositions_X() != nullptr && g_draw_realistic) {
            g_particle_renderer->UpdateParticlePositionsTexture(
                dynamic_cast<GPU_Simulation*>(g_sim)->GetTexParticlePositions_X(), 
                dynamic_cast<GPU_Simulation*>(g_sim)->GetTexParticlePositions_Y(), 
                dynamic_cast<GPU_Simulation*>(g_sim)->GetTexParticlePositions_Z()
            );
            g_particle_renderer->Draw();
        }
        g_skybox->Draw();
        g_debug_renderer->Draw(enable_particles);

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
    printf("Just before update begins\n");
    UpdateLoop();
    glfwTerminate();

    delete g_sim;
    delete g_cam;
    delete g_debug_renderer;
    delete g_skybox;

	return 0;
}