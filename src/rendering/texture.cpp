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

Texture::Texture(int desired_channels, int dimensions, const float* data) : texture_obj_id_(0), channels_(desired_channels)
{
    if (desired_channels <= 0 || desired_channels > 4) 
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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, dimensions, dimensions, 0, GL_RGBA, GL_FLOAT, data);
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

    printf("Dimensions of texture image is %d %d with %d channels.\n", width, height, channels);
    channels_ = 4;
    if (pixels != nullptr)
    {
        glGenTextures(1, &texture_obj_id_);
        glBindTexture(GL_TEXTURE_2D, texture_obj_id_);

        glTexStorage2D(GL_TEXTURE_2D, 2 /* mip map levels */, GL_RGB8, width, height);
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

void Texture::ActiveBind(GLint texture_unit)
{
    if (texture_obj_id_ != 0)
    {
        glActiveTexture(texture_unit);
        glBindTexture(GL_TEXTURE_2D, texture_obj_id_);
    }
}

void Texture::BindImage(unsigned int binding) 
{
    glBindImageTexture(binding, texture_obj_id_, 0, 0, 0, GL_READ_WRITE, GL_RGBA32F);
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
    glTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, width, height, GL_RGBA, GL_FLOAT, pixel_data);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
}
