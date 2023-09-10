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

    /* Set the viewport */
    glClearColor(0.6784f, 0.8f, 1.0f, 1.0f);
    glViewport(0, 0, kWindowWidth, kWindowHeight);

    glEnable(GL_DEPTH_TEST);

    return true;
}

void QuadTextureSetup(GLuint& vert_array, GLuint& vert_buffer, GLuint& uv_buffer, GLuint& index_buffer) 
{
    const std::vector<glm::vec3> kQuadVerts = { glm::vec3(-1.0, -1.0, 0.0),
                                            glm::vec3(1.0, -1.0, 0.0),
                                            glm::vec3(-1.0, 1.0, 0.0),
                                            glm::vec3(1.0, 1.0, 0.0) };

    const std::vector<glm::vec2> kQuadUVs = { glm::vec2(0, 0),
                                                glm::vec2(1, 0),
                                                glm::vec2(0, 1),
                                                glm::vec2(1, 1) };

    const std::vector<unsigned short> kQuadIndices = { 0, 1, 2, 2, 1, 3 };

    // VAO for the vertices
    glGenVertexArrays(1, &vert_array);
    glBindVertexArray(vert_array);

    // Gen vert buffer
    glGenBuffers(1, &vert_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vert_buffer);
    glBufferData(GL_ARRAY_BUFFER, kQuadVerts.size() * sizeof(glm::vec3), &kQuadVerts[0], GL_STATIC_DRAW);

    // Gen UV buffer
    glGenBuffers(1, &uv_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, kQuadUVs.size() * sizeof(glm::vec2), &kQuadUVs[0], GL_STATIC_DRAW);

    // Gen index buffer
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ARRAY_BUFFER, kQuadIndices.size() * sizeof(unsigned short), &kQuadIndices[0], GL_STATIC_DRAW);
}

void QuadTextureRender(GLuint vert_buffer, GLuint uv_buffer, GLuint index_buffer) 
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vert_buffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glVertexAttribPointer(
        1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        2,                  // size
        GL_FLOAT,				// type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    // Draw the triangles !
    glDrawElements(
        GL_TRIANGLES,      // mode
        6,    // count
        GL_UNSIGNED_SHORT, // type
        (void*)0           // element array buffer offset
    );
}

void UpdateLoop() 
{
    float start_time = static_cast<float>(glfwGetTime());
    float previous_time = start_time;
    float new_time = 0.0f;
    float game_time = 0.0f; // Time the sim has been running

    struct ComputeData {
        float time;
        float deltaTime;
    };
    struct ComputeData c_data;
    c_data.time = game_time;
    c_data.deltaTime = 0;

    ComputeShader compute_shader("waterflow_shader.comp", glm::ivec3(512, 512, 1));
    GLuint ssbo = compute_shader.GenerateAndBindSSBO(&c_data, 8, 0);
    std::vector<float> pix_data(512 * 512 * 4, 0.0);
    for (int i = 150000; i < 180000; i++) {
        pix_data[i] = 1.0;
    }
    Texture diffusion_old(4, 512, &pix_data[0]);
    Texture diffusion_new(4, 512);

    diffusion_new.ActiveBind(GL_TEXTURE1);
    diffusion_old.ActiveBind(GL_TEXTURE2);

    Shader quad_shader("flat_quad_shader.vert", "flat_quad_shader.frag");
    GLuint vert_array, vert_buffer, uv_buffer, index_buffer;
    QuadTextureSetup(vert_array, vert_buffer, uv_buffer, index_buffer);



    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Update game time value */
        new_time = static_cast<float>(glfwGetTime());
        c_data.deltaTime = new_time - previous_time;
        previous_time = new_time;
        game_time = new_time - start_time;
        c_data.time = game_time;

        ////////////////////
        // Compute Shader //
        ////////////////////
        compute_shader.SetActive();
        compute_shader.UpdateSSBO(ssbo, &c_data, 8, 0);
        diffusion_old.BindImage(1);
        diffusion_new.BindImage(2);
        compute_shader.Dispatch();
        compute_shader.Barrier();

        ////////////////////
        // Flat Rendering //
        ////////////////////
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        quad_shader.SetActive();
        diffusion_new.ActiveBind(GL_TEXTURE0);
        // quad_shader.SetUniformTexture("tex", diffusion_old, GL_TEXTURE0);
        QuadTextureRender(vert_buffer, uv_buffer, index_buffer);

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