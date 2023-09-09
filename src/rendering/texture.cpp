#include "texture.hpp"
#include <RootDir.h>

#include <stb_image.h>


///////////////////////
///	Private Methods ///
///////////////////////

// Code for creating a Texture object are from Tomasz Ga'aj's
// OpenGL CMake tutorial, modified to this project's use-case
// Specifically, this version of texture allows for the creation
// of empty textures and the future modification of those textures.

//////////////////////
///	Public Methods ///
//////////////////////

Texture::Texture(int desired_channels, int dimensions) : texture_obj_id_(0) 
{
    if (desired_channels <= 0 || desired_channels > 4) 
    {
        fprintf(stderr, "Texture created with %d number of channels instead of between 1 and 4\n", desired_channels);
        return;
    }
    glGenTextures(1, &texture_obj_id_);
    glBindTexture(GL_TEXTURE_2D, texture_obj_id_);
    glTexImage2D(GL_TEXTURE_2D, 0, kTextureChannels_[desired_channels - 1], 
            dimensions, dimensions, 0, kTextureChannels_[desired_channels - 1], GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

Texture::Texture(const std::string& filename) : texture_obj_id_(0)
{
    if (filename.empty())
    {
        fprintf(stderr, "Failure creating Texture obj, file name provided is empty.\n");
        return;
    }

    bool is_loaded = false;
    int width, height, channels;

    unsigned char* pixels = stbi_load((ROOT_DIR "resources/textures/" + filename).c_str(), &width, &height, &channels, 4);

    if (pixels != nullptr)
    {
        glGenTextures(1, &texture_obj_id_);
        glBindTexture(GL_TEXTURE_2D, texture_obj_id_);

        glTexStorage2D(GL_TEXTURE_2D, 2 /* mip map levels */, kTextureStorageFormat_[channels - 1], width, height);
        glTexSubImage2D(GL_TEXTURE_2D, 0 /* mip map level */, 0 /* xoffset */, 0 /* yoffset */, width, height, kTextureChannels_[channels - 1], GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D); // Replaces mipmap levels from 0 to 2 with generated mip maps

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else
    {
        fprintf(stderr, "Texture \"%s\" could not be loaded. Is the texture file in \"resources/textures/\"?\n", filename);
    }

    stbi_image_free(pixels);
}

void Texture::ActiveBind(GLint texture_unit)
{
    if (texture_obj_id_ != 0)
    {
        glActiveTexture(texture_unit);
        glBindTexture(GL_TEXTURE_2D, texture_obj_id_);
    }
}

void Texture::GetTextureId() 
{
    return texture_obj_id_;
}