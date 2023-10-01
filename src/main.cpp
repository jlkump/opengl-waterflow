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
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"
#include "rendering/compute_shader.hpp"
#include "rendering/model.h"
#include "rendering/cubemap.hpp"
#include "rendering/camera.hpp"
#include "simulation/water_particle_renderer.hpp"

GLFWwindow* window;
const int kWindowWidth = 1024;
const int kWindowHeight = 768;

Model* g_model = nullptr;
Shader* g_shader = nullptr;
Camera* g_camera = nullptr;
bool g_simulate = false;

glm::mat4 model_matrix = glm::mat4(1.0f);

void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    if (g_shader != nullptr && g_camera != nullptr)
    {
        g_camera->SetAspectRatio(width, height);
        g_shader->SetUniformMatrix4fv("proj_view", g_camera->GetProjectionMatrix() * g_camera->GetViewMatrix());
    }
}

void PrintMatrix(glm::mat4& mat) {
    for (int x = 0; x < 4; x++) {
        printf("[");
        for (int y = 0; y < 4; y++) {
            printf(" %.2f ", mat[y][x]);
        }
        printf("]\n");
    }
    printf("\n");
}

void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //static float param = 0.0f;
    //static float radius = 1.0f;
    //static float height = 0.0f;

    // Model rotation
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        model_matrix = glm::rotate(model_matrix, glm::radians(-45.0f), glm::vec3(0, 1, 0));
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        model_matrix = glm::rotate(model_matrix, glm::radians(45.0f), glm::vec3(0, 1, 0));

    //// Camara controls
    //if (key == GLFW_KEY_L && action == GLFW_PRESS)
    //    param += 0.5f;
    //if (key == GLFW_KEY_J && action == GLFW_PRESS)
    //    param -= 0.5f;
    //if (key == GLFW_KEY_I && action == GLFW_PRESS)
    //    radius += 0.5f;
    //if (key == GLFW_KEY_K && action == GLFW_PRESS)
    //    radius -= 0.5f;
    //if (key == GLFW_KEY_U && action == GLFW_PRESS)
    //    height -= 0.5f;
    //if (key == GLFW_KEY_O && action == GLFW_PRESS)
    //    height += 0.5f;

    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        g_simulate = !g_simulate;
    //g_camera->SetPosition(glm::vec3(sin(param) * radius, 1.0f + height, cos(param) * radius));
    //g_shader->SetUniform3fv("ws_cam_pos", g_camera->GetPosition());

    //g_shader->SetUniformMatrix4fv("model", model_matrix);
    //g_shader->SetUniformMatrix3fv("norm_matrix", glm::inverse(glm::transpose(glm::mat3(model_matrix))));
    //g_shader->SetUniformMatrix4fv("proj_view", g_camera->GetProjectionMatrix() * g_camera->GetViewMatrix());
    //if (action == GLFW_PRESS)
    //    PrintMatrix(g_camera->GetViewMatrix());
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

    /* Create and apply basic shader */
    g_shader = new Shader("basic.vert", "basic_reflection.frag");
    g_shader->SetUniformMatrix4fv("model", model_matrix);
    g_shader->SetUniformMatrix3fv("norm_matrix", glm::inverse(glm::transpose(glm::mat3(model_matrix))));
    g_shader->SetUniformMatrix4fv("proj_view", g_camera->GetProjectionMatrix() * g_camera->GetViewMatrix());

    g_shader->SetUniform3fv("ws_cam_pos", g_camera->GetPosition());

    g_model = new Model("resources/models/alliance.obj");

    return true;
}

// Temp function for initialization
void InitializeParticles(std::vector<glm::vec4>& particle_positions, const glm::vec3& lower_bound, const glm::vec3& upper_bound) {
    static const double delta = 0.125f / 4.0;
    int i = 0;
    for (double x = lower_bound.x; x < upper_bound.x; x += delta) {
        for (double y = lower_bound.y; y < upper_bound.y; y += delta) {
            for (double z = lower_bound.z; z < upper_bound.z; z += delta) {
                if (i < MAX_NUM_PARTICLES) {
                    particle_positions[i] = glm::vec4(x, y, z, 1.0);
                    //printf("Pos %.2f, %.2f, %.2f\n", x, y, z);
                }
                i++;
            }
        }
    }
}

void UpdateLoop() 
{
    float previous_time = static_cast<float>(glfwGetTime());
    float new_time = 0.0f;

    // Skybox setup
    Skybox skybox({ "skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg" });

    WaterParticleRenderer* pic_flip_renderer = new WaterParticleRenderer();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    struct BOUNDS {
        glm::vec3 upper_bound;
        glm::vec3 lower_bound;
    };

    struct BOUNDS bounds;
    bounds.upper_bound = glm::vec3(1,1,1);
    bounds.lower_bound = glm::vec3(0, 0, 0);
    std::vector<glm::vec4> particle_pos(512 * 512);
    InitializeParticles(particle_pos, bounds.lower_bound, bounds.upper_bound);
    glm::ivec2 particle_tex_dimen(512, 512);
    Texture tex_pos_old(particle_tex_dimen, GL_RGBA, GL_RGBA32F, (const float*) &particle_pos[0]);
    Texture tex_pos_new(particle_tex_dimen, GL_RGBA, GL_RGBA32F, (const float*) &particle_pos[0]);
    Texture tex_vel_old(particle_tex_dimen);
    Texture tex_vel_new(particle_tex_dimen);

    GLuint lock_texture;
    glGenTextures(1, &lock_texture);
    glBindTexture(GL_TEXTURE_3D, lock_texture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, 512, 512, 512, 0, GL_RED, GL_UNSIGNED_INT, 0);

    GLuint grid_texture;
    glGenTextures(1, &grid_texture);
    glBindTexture(GL_TEXTURE_3D, grid_texture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, 512, 512, 512, 0, GL_RGBA, GL_FLOAT, 0);


    ComputeShader simulation("pic_flip_shader.comp", glm::ivec3(particle_tex_dimen.x, particle_tex_dimen.y, 1));
    GLuint bound_ssbo = simulation.GenerateAndBindSSBO(&bounds, sizeof(BOUNDS), 0);
    //simulation.SetUniform1fv("particleRadius", 0.3);

    /* Loop until the user closes the window or presses ESC */
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window))
    {
        /* Update game time value */
        new_time = static_cast<float>(glfwGetTime());
        float deltaTime = new_time - previous_time;
        previous_time = new_time;

        ////////////////
        // Simulation //
        ////////////////
        if (g_simulate) {
            simulation.SetActive();
            simulation.SetUniform1fv("deltaTime", deltaTime);
            tex_pos_old.BindImage(1);
            tex_pos_new.BindImage(2);
            tex_vel_old.BindImage(3);
            tex_vel_new.BindImage(4);
            glBindImageTexture(5, grid_texture, 0, 0, 0, GL_READ_WRITE, GL_FLOAT);
            glBindImageTexture(6, lock_texture, 0, 0, 0, GL_READ_WRITE, GL_UNSIGNED_INT);
            simulation.Dispatch();
            simulation.Barrier();
        }

        ////////////////////
        //  3D Rendering  //
        ////////////////////
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        skybox.ActiveBind(GL_TEXTURE0);
        skybox.Draw(g_camera->GetViewMatrix(), g_camera->GetProjectionMatrix());
        pic_flip_renderer->UpdateParticlePositionsTexture(tex_pos_old);
        pic_flip_renderer->Draw(*g_camera, skybox);

        // g_shader->SetActive();
        // g_model->Draw();


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    delete pic_flip_renderer;
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

    delete g_shader;
    delete g_model;
    delete g_camera;

	return 0;
}