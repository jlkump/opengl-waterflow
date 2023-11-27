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

GLFWwindow* window;
const int kWindowWidth = 1024;
const int kWindowHeight = 768;

FPSCamera* g_cam = nullptr;
Skybox* g_skybox = nullptr;
DebugRenderer* g_debug_renderer = nullptr;
Simulation* g_seq_sim = nullptr;
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
    if (key == GLFW_KEY_P) {
        if (action == GLFW_PRESS) {
            g_simulate = !g_simulate;
        }
    }
    if (key == GLFW_KEY_1) {
        if (action == GLFW_PRESS) {
            if (g_seq_sim != nullptr && g_seq_sim->GetGridPressures() != nullptr) {
                if (g_debug_renderer->IsCellViewActive(DebugRenderer::PRESSURE) || !g_debug_renderer->IsDebugViewActive(DebugRenderer::GRID_CELL)) {
                    g_debug_renderer->ToggleDebugView(DebugRenderer::GRID_CELL);
                }
                g_debug_renderer->SetGridPressures(*g_seq_sim->GetGridPressures(), g_seq_sim->GetGridDimensions());
            }
        }
    }
    if (key == GLFW_KEY_2) {
        if (action == GLFW_PRESS) {
            if (g_seq_sim != nullptr && g_seq_sim->GetGridDyeDensities() != nullptr) {
                if (g_debug_renderer->IsCellViewActive(DebugRenderer::DYE) || !g_debug_renderer->IsDebugViewActive(DebugRenderer::GRID_CELL)) {
                    g_debug_renderer->ToggleDebugView(DebugRenderer::GRID_CELL);
                }
                g_debug_renderer->SetGridDyeDensities(*g_seq_sim->GetGridDyeDensities(), g_seq_sim->GetGridDimensions());
            }
        }
    }
    if (key == GLFW_KEY_3) {
        if (action == GLFW_PRESS) {
            if (g_seq_sim != nullptr && g_seq_sim->GetGridFluidCells() != nullptr) {
                printf("Displaying fluid cells\n");
                if (g_debug_renderer->IsCellViewActive(DebugRenderer::IS_FLUID) || !g_debug_renderer->IsDebugViewActive(DebugRenderer::GRID_CELL)) {
                    printf("Toggling cell display\n");
                    g_debug_renderer->ToggleDebugView(DebugRenderer::GRID_CELL);
                }
                g_debug_renderer->SetGridFluidCells(*g_seq_sim->GetGridFluidCells(), g_seq_sim->GetGridDimensions());
            }
        }
    }

    // Toggle particles
    if (key == GLFW_KEY_4) {
        if (action == GLFW_PRESS) {
            g_debug_renderer->ToggleDebugView(DebugRenderer::PARTICLES);
        }
    }

    // Toggle particle vectors
    if (key == GLFW_KEY_5) {
        if (action == GLFW_PRESS) {
            g_debug_renderer->ToggleDebugView(DebugRenderer::PARTICLE_VELOCITIES);
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
    glEnable(GL_BLEND);
    glDepthFunc(GL_LESS);

    return true;
}

bool LoadContent()
{
    /* Create camera for scene */
    g_cam = new FPSCamera(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, 0.0f, -1.0f));

    /* Create Skybox for scene */
    g_skybox = new Skybox({ "skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg" });

    //g_seq_sim = new SequentialGridBased();
    g_seq_sim = new SequentialParticleBased();
    std::vector<glm::vec3> initialParticleVelocities(8 * 8);
    for (size_t i = 0; i < initialParticleVelocities.size(); ++i) {
        initialParticleVelocities[i] = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    g_seq_sim->SetInitialVelocities(
        initialParticleVelocities, 
        glm::vec3(-1.0, -1.0, -1.0), 
        glm::vec3(1.0, 1.0, 1.0), 
        0.5f
    );
    glm::ivec2 particle_tex_dimen(8, 8);
    std::vector<glm::vec3>& temp_positions = *g_seq_sim->GetParticlePositions();
    std::vector<glm::vec3>& temp_velocities = *g_seq_sim->GetParticleVelocities();

    std::vector<glm::vec4> positions;
    //std::vector<glm::vec4> velocities(temp_velocities.size());
    for (const glm::vec3& pos : temp_positions) {
        //positions.emplace_back(pos.x, pos.y, pos.z, 1);
        positions.emplace_back(0, 0, 0, 1);
    }
    for (const glm::vec4& pos : positions) {
        printf("\t- pos = (%f, %f, %f, %f)\n", pos.x, pos.y, pos.z, pos.w);
    }

    Texture2D texture_positions(particle_tex_dimen, StorageType::TEX_FLOAT, ChannelType::RGBA32F, (const float*)&positions[0]);
    //Texture2D texture_velocities(particle_tex_dimen, StorageType::TEX_FLOAT, ChannelType::RGBA32F, (const float*)&velocities[0]);
    printf("Number of particle positions = %ld\n", positions.size());
    //printf("Number of particle velocities = %ld\n", velocities.size());

    g_debug_renderer = new DebugRenderer();
    g_debug_renderer->SetGridBoundaries(g_seq_sim->GetGridLowerBounds(), g_seq_sim->GetGridUpperBounds(), g_seq_sim->GetGrindInterval());
    g_debug_renderer->SetGridVelocities(*g_seq_sim->GetGridVelocities(), g_seq_sim->GetGridDimensions());
    g_debug_renderer->SetParticlePositions(texture_positions);
    //g_debug_renderer->SetParticleVelocities(texture_velocities);


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

    // TODO: START remove ---
    //SequentialParticleBased* temp = new SequentialParticleBased();
    //std::vector<glm::vec3> initialParticleVelocities(4 * 4 * 4);
    //for (size_t i = 0; i < initialParticleVelocities.size(); ++i) {
    //    initialParticleVelocities[i] = glm::vec3(0.0f, 0.0f, 0.0f);
    //}
    //temp->SetInitialVelocities(initialParticleVelocities, glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1), 0.5f);

    //WaterParticleRenderer* pic_flip_renderer = new WaterParticleRenderer();

    //std::vector<glm::vec3> particle_pos(512 * 512);
    //for (size_t i = 0; i < particle_pos.size(); ++i) {
    //    particle_pos[i] = glm::vec4(1.0,1.0,1.0, 1);
    //}

    //std::vector<glm::vec3>* positions = temp->GetParticlePositions();
    //std::vector<glm::vec3>* velocities = temp->GetParticleVelocities();
    //printf("Number of particle positions = %ld\n", positions->size());
    //printf("Number of particle velocities = %ld\n", velocities->size());
    //for (const auto pos : *positions) {
    //    printf("\tpos: (%f, %f, %f)\n", pos.x, pos.y, pos.z);
    //}

    //glm::ivec2 particle_tex_dimen(64, 64);
    //Texture2D tex_pos_old(particle_tex_dimen, StorageType::TEX_FLOAT, ChannelType::RGBA32F, (const float*)&positions[0]);
    //Texture2D tex_pos_new(particle_tex_dimen, StorageType::TEX_FLOAT, ChannelType::RGBA32F, (const float*)&positions[0]);
    //Texture2D tex_vel_old(particle_tex_dimen);
    //Texture2D tex_vel_new(particle_tex_dimen);

    //system("pause");
    // TODO: END remove ---

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
        if (g_keys_pressed.size() != 0) {
            UpdateView(g_cam->GetCam()->GetViewMatrix());
            UpdateProjection(g_cam->GetCam()->GetProjectionMatrix());
        }
        if (new_time - last_time_updated >= time_step && g_simulate) {
            // Perform new step in simulation
            g_seq_sim->TimeStep(deltaTime);
            g_debug_renderer->SetGridVelocities(*g_seq_sim->GetGridVelocities(), g_seq_sim->GetGridDimensions());
            if (g_debug_renderer->IsDebugViewActive(DebugRenderer::GRID_CELL)) {
                switch (g_debug_renderer->GetCellViewActive()) {
                case DebugRenderer::DYE:
                    g_debug_renderer->SetGridDyeDensities(*g_seq_sim->GetGridDyeDensities(), g_seq_sim->GetGridDimensions());
                    break;
                case DebugRenderer::IS_FLUID:
                    g_debug_renderer->SetGridDyeDensities(*g_seq_sim->GetGridFluidCells(), g_seq_sim->GetGridDimensions());
                    break;
                case DebugRenderer::PRESSURE:
                    g_debug_renderer->SetGridPressures(*g_seq_sim->GetGridPressures(), g_seq_sim->GetGridDimensions());
                    break;
                case DebugRenderer::NONE:
                    break;
                }
            }
            last_time_updated = new_time;
        }

        ////////////////////
        //  3D Rendering  //
        ////////////////////
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        g_debug_renderer->Draw(*(g_cam->GetCam()), *g_skybox);
        g_skybox->Draw();
        //pic_flip_renderer->UpdateParticlePositionsTexture(tex_pos_old);
        //pic_flip_renderer->Draw(*(g_cam->GetCam()), *g_skybox);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    //delete pic_flip_renderer;
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