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

Texture2D::Texture2D(glm::ivec2 dimensions, StorageType storage_type, ChannelType channel_type, const void* initial_data) :
	dimensions_(dimensions), storage_type_(storage_type), channel_type_(channel_type),
	gl_storage_type_(GL_NONE), gl_channel_type_(GL_NONE), valid_texture_(false), texture_id_(0)
{
	// Generate handle for texture on GPU
	glGenTextures(1, &texture_id_);
	// Bind the texture to the active texture. Future texture calls modify this texture
	glBindTexture(GL_TEXTURE_2D, texture_id_);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// TODO: figure out format and internal format for glTexImage2D(...)
	GLenum format = GL_RGBA;

	switch (channel_type) {
		case ChannelType::R:
			gl_channel_type_ = GL_RED;
			format = GL_RED;
			break;
		case ChannelType::RG:
			gl_channel_type_ = GL_RG;
			format = GL_RG;
			break;
		case ChannelType::RGB:
			gl_channel_type_ = GL_RGB;
			format = GL_RGB;
			break;
		case ChannelType::RGB32F:
			gl_channel_type_ = GL_RGB32F;
			format = GL_RGB;
			break;
		case ChannelType::RGBA:
			gl_channel_type_ = GL_RGBA;
			format = GL_RGBA;
			break;
		case ChannelType::RGBA32F:
			gl_channel_type_ = GL_RGBA32F;
			format = GL_RGBA;
			break;
	}

	switch (storage_type) {
		case StorageType::TEX_BYTE:
			gl_storage_type_ = GL_BYTE;
			break;
		case StorageType::TEX_SHORT:
			gl_storage_type_ = GL_SHORT;
			break;
		case StorageType::TEX_INT:
			gl_storage_type_ = GL_INT;
			break;
		case StorageType::TEX_FLOAT:
			gl_storage_type_ = GL_FLOAT;
			break;
	}
	// Place texture data to the GPU
	glTexImage2D(GL_TEXTURE_2D, 0, gl_channel_type_, dimensions.x, dimensions.y, 0, format, gl_storage_type_, initial_data);
	// TODO: Error check the result of OpenGL calls
	valid_texture_ = true;
}

Texture2D::Texture2D(const std::string& texture_filename) : 
	dimensions_(glm::ivec2(0,0)), storage_type_(StorageType::TEX_BYTE), channel_type_(ChannelType::RGBA),
	gl_storage_type_(GL_BYTE), gl_channel_type_(GL_RGBA), valid_texture_(false), texture_id_(0)
{
	if (texture_filename.empty())
	{
		fprintf(stderr, "Failure creating Texture obj, file name provided is empty.\n");
		return;
	}

	unsigned char* pixels = stbi_load((ROOT_DIR "resources/textures/" + texture_filename).c_str(), &dimensions_.x, &dimensions_.y, nullptr, 4);

	if (pixels != nullptr)
	{
		glGenTextures(1, &texture_id_);
		glBindTexture(GL_TEXTURE_2D, texture_id_);

		glTexStorage2D(GL_TEXTURE_2D, 2 /* mip map levels */, GL_RGBA8, dimensions_.x, dimensions_.y);
		glTexSubImage2D(GL_TEXTURE_2D, 0 /* mip map level */, 0 /* xoffset */, 0 /* yoffset */, dimensions_.x, dimensions_.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glGenerateMipmap(GL_TEXTURE_2D); // Replaces mipmap levels from 0 to 2 with generated mip maps

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		fprintf(stderr, "Texture \"%s\" could not be loaded. Is the texture file in \"resources/textures/\"?\n", texture_filename.c_str());
		return;
	}

	valid_texture_ = true;

	stbi_image_free(pixels);
}

Texture2D::~Texture2D()
{
    if (texture_id_ != 0)
    {
        glDeleteTextures(1, &texture_id_);
    }
}

GLuint Texture2D::GetTextureId() const
{
	return texture_id_;
}

bool Texture2D::ModifyTextureData(glm::ivec2 top_left_start, glm::ivec2 data_dimensions, const void* texture_data)
{
	if (!valid_texture_) {
		return false;
	}
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, data_dimensions.x);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	switch (storage_type_) {
	case TEX_BYTE:
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		break;
	case TEX_SHORT:
		glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		break;
	case TEX_INT:
	case TEX_FLOAT:
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		break;
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, 
		top_left_start.x, top_left_start.y, 
		data_dimensions.x, data_dimensions.y, 
		gl_channel_type_, gl_storage_type_, texture_data);

	return true;
}

glm::ivec2 Texture2D::GetDimensions() const
{
	return dimensions_;
}

GLenum Texture2D::GetGLStorageType() const
{
	return storage_type_;
}

GLenum Texture2D::GetGLChannelType() const
{
	return channel_type_;
}

bool Texture2D::ActiveBind(GLenum texture_unit_binding)
{
	if (!valid_texture_) {
		return false;
	}
    glActiveTexture(texture_unit_binding);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
	return true;
}

bool Texture2D::BindImageTexture(GLenum texture_unit_binding)
{
	if (!valid_texture_) {
		return false;
	}
	glBindImageTexture(texture_unit_binding, texture_id_, 0, 0, 0, GL_READ_WRITE, gl_storage_type_);
	return true;
}



Texture3D::Texture3D(glm::ivec3 dimensions, StorageType storage_type, ChannelType channel_type, const void* initial_data)
	: gl_storage_type_(GL_NONE), gl_channel_type_(GL_NONE), valid_texture_(false), texture_id_(0)
{
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_3D, texture_id_);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	switch (channel_type) {
	case ChannelType::R:
		gl_channel_type_ = GL_RED;
		break;
	case ChannelType::RG:
		gl_channel_type_ = GL_RG;
		break;
	case ChannelType::RGB:
		gl_channel_type_ = GL_RGB;
		break;
	case ChannelType::RGBA:
		gl_channel_type_ = GL_RGBA;
		break;
	}

	switch (storage_type) {
	case StorageType::TEX_BYTE:
		gl_storage_type_ = GL_BYTE;
		break;
	case StorageType::TEX_SHORT:
		gl_storage_type_ = GL_SHORT;
		break;
	case StorageType::TEX_INT:
		gl_storage_type_ = GL_INT;
		break;
	case StorageType::TEX_FLOAT:
		gl_storage_type_ = GL_FLOAT;
		break;
	}

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, dimensions.x, dimensions.y, dimensions.z, 0, GL_RGBA, GL_FLOAT, 0);

	valid_texture_ = true;
}

Texture3D::~Texture3D()
{
	if (texture_id_ != 0)
	{
		glDeleteTextures(1, &texture_id_);
	}
}

GLuint Texture3D::GetTextureId() const
{
	return texture_id_;
}

bool Texture3D::ModifyTextureData(glm::ivec3 top_left_start, glm::ivec3 data_dimensions, const void* texture_data)
{
	if (!valid_texture_) {
		return false;
	}
	glBindTexture(GL_TEXTURE_3D, texture_id_);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, data_dimensions.x);
	glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, data_dimensions.y);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	switch (storage_type_) {
	case TEX_BYTE:
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		break;
	case TEX_SHORT:
		glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		break;
	case TEX_INT:
	case TEX_FLOAT:
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		break;
	}
	glTexSubImage3D(GL_TEXTURE_3D, 0, 
		top_left_start.x, top_left_start.y, top_left_start.z, 
		data_dimensions.x, data_dimensions.y, data_dimensions.z, 
		gl_channel_type_, gl_storage_type_, texture_data);
	return true;
}

glm::ivec3 Texture3D::GetDimensions() const
{
	return dimensions_;
}

GLenum Texture3D::GetGLStorageType() const
{
	return storage_type_;
}

GLenum Texture3D::GetGLChannelType() const
{
	return channel_type_;
}

bool Texture3D::ActiveBind(GLenum texture_unit_binding)
{
	if (!valid_texture_) {
		return false;
	}
	glActiveTexture(texture_unit_binding);
	glBindTexture(GL_TEXTURE_2D, texture_id_);
	return true;
}

bool Texture3D::BindImageTexture(GLenum texture_unit_binding)
{
	if (!valid_texture_) {
		return false;
	}
	glBindImageTexture(texture_unit_binding, texture_id_, 
		0,		// level to bind
		false,	// T/F texture is layered
		0,		// layer to bind if layered is T
		GL_READ_WRITE, storage_type_);
	return true;
}



// Code modified from https://learnopengl.com/Advanced-OpenGL/Cubemaps
Cubemap::Cubemap(std::vector<std::string> input_textures)
	: Texture2D()
{
	glGenTextures(1, &texture_id_);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);

	int nrChannels;
	for (unsigned int i = 0; i < input_textures.size(); i++)
	{
		unsigned char* data = stbi_load((ROOT_DIR "resources/textures/" + input_textures[i]).c_str(), &dimensions_.x, &dimensions_.y, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, dimensions_.x, dimensions_.y, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			gl_storage_type_ = GL_UNSIGNED_BYTE;
			gl_channel_type_ = GL_RGB;
			storage_type_ = StorageType::TEX_BYTE;
			channel_type_ = ChannelType::RGB;
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
	valid_texture_ = true;
}


Cubemap::~Cubemap()
{
	if (texture_id_ != 0)
	{
		glDeleteTextures(1, &texture_id_);
	}
}

bool Cubemap::ActiveBind(GLenum texture_unit)
{
	if (!valid_texture_)
	{
		return false;
	}
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
	return true;
}