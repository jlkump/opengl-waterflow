#include "cubemap.hpp"
#include <RootDir.h>
#include <stb_image.h>

// Code modified from https://learnopengl.com/Advanced-OpenGL/Cubemaps
Cubemap::Cubemap(std::vector<std::string> input_textures) 
	: Texture()
{
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < input_textures.size(); i++)
    {
        unsigned char* data = stbi_load((ROOT_DIR "resources/textures/" + input_textures[i]).c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            fprintf(stderr, "Failure loading cubemap texture at location %s.\nMake sure it is in \"resources/textures\"", input_textures[i].c_str());
            stbi_image_free(data); // IDK why you would need to call with nullptr, but opengl tutorial does it :/
            valid_texture_ = false;
            return;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}


Cubemap::~Cubemap()
{
    if (texture_id_ != 0)
    {
        glDeleteTextures(1, &texture_id_);
    }
}

Skybox::Skybox(std::vector<std::string> input_textures) 
    : Cubemap(input_textures), skybox_shader_("skybox.vert", "skybox.frag"), VAO_(0)
{
    unsigned int VBO;

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), &kSkyboxVertices_[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0); // Unbind this vertex array

    skybox_shader_.SetUniformTexture("skybox", *this, GL_TEXTURE0);
}

void Skybox::SetShader(Shader& s) {
    skybox_shader_ = s;
}

void Skybox::Draw(glm::mat4 view, glm::mat4 projection)
{
    if (!valid_texture_) 
    {
        return;
    }
    glDepthFunc(GL_LEQUAL);
    skybox_shader_.SetActive();
    skybox_shader_.SetUniformMatrix4fv("proj_view", projection * glm::mat4(glm::mat3(view)));
    glBindVertexArray(VAO_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
