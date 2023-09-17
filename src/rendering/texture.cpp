#include "texture.hpp"
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

Texture::Texture(int dimensions, GLenum desired_channels, GLenum storage_type, const float* data) 
    : texture_obj_id_(0), channels_(desired_channels), storage_type_(storage_type)
{
    if (desired_channels < GL_RED || desired_channels > GL_RGBA) 
    {
        fprintf(stderr, "Texture created with %d number of channels instead of between 1 and 4\n", desired_channels);
        return;
    }
    glGenTextures(1, &texture_obj_id_);
    glBindTexture(GL_TEXTURE_2D, texture_obj_id_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    switch (desired_channels) {
    case GL_RED:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dimensions, dimensions, 0, GL_RED, GL_FLOAT, data);
        break;
    case GL_RG:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, dimensions, dimensions, 0, GL_RG, GL_FLOAT, data);
        break;
    case GL_RGB:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, dimensions, dimensions, 0, GL_RGB, GL_FLOAT, data);
        break;
    case GL_RGBA:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, dimensions, dimensions, 0, GL_RGBA, GL_FLOAT, data);
        break;
    };
}

Texture::Texture(const std::string& filename) 
    : texture_obj_id_(0), channels_(GL_RGBA), storage_type_(GL_RGBA8)
{
    if (filename.empty())
    {
        fprintf(stderr, "Failure creating Texture obj, file name provided is empty.\n");
        return;
    }

    bool is_loaded = false;
    int width, height;

    unsigned char* pixels = stbi_load((ROOT_DIR "resources/textures/" + filename).c_str(), &width, &height, nullptr, 4);

    if (pixels != nullptr)
    {
        glGenTextures(1, &texture_obj_id_);
        glBindTexture(GL_TEXTURE_2D, texture_obj_id_);

        glTexStorage2D(GL_TEXTURE_2D, 2 /* mip map levels */, GL_RGBA8, width, height);
        glTexSubImage2D(GL_TEXTURE_2D, 0 /* mip map level */, 0 /* xoffset */, 0 /* yoffset */, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D); // Replaces mipmap levels from 0 to 2 with generated mip maps

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else
    {
        fprintf(stderr, "Texture \"%s\" could not be loaded. Is the texture file in \"resources/textures/\"?\n", filename.c_str());
    }

    stbi_image_free(pixels);
}

Texture::~Texture() {
    if (texture_obj_id_ != 0)
    {
        glDeleteTextures(1, &texture_obj_id_);
    }
}

void Texture::ActiveBind(GLenum texture_unit)
{
    if (texture_obj_id_ != 0)
    {
        glActiveTexture(texture_unit);
        glBindTexture(GL_TEXTURE_2D, texture_obj_id_);
    }
}

void Texture::BindImage(unsigned int binding) 
{
    glBindImageTexture(binding, texture_obj_id_, 0, 0, 0, GL_READ_WRITE, storage_type_);
}

GLuint Texture::GetTextureId() 
{
    return texture_obj_id_;
}

void Texture::UpdatePixelData(GLint x_offset, GLint y_offset, GLsizei width, GLsizei height, const void* pixel_data)
{

    glBindTexture(GL_TEXTURE_2D, texture_obj_id_);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if (storage_type_ == GL_R8 || storage_type_ == GL_RG8 || storage_type_ == GL_RGB8 || storage_type_ == GL_RGBA8) 
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, width, height, channels_, GL_BYTE, pixel_data);
    }
    else 
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, width, height, channels_, GL_FLOAT, pixel_data);
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
}
