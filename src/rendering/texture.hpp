#ifndef TEXTURE_C
#define TEXTURE_C

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

enum StorageType {
	TEX_FLOAT,
	TEX_BYTE,
	TEX_SHORT,
	TEX_UINT,
	TEX_INT,
};

enum ChannelType {
	R,
	R32F,
	R32I,
	R32UI,
	RG,
	RG32F,
	RGB,
	RGB32F,
	RGBA,
	RGBA32F
};

class Shader;

class Texture2D {
private:
protected:
	/*
	* @brief
	* Tracks the texture created on the GPU with a texture id
	* Manipulating the texture on the GPU will require this id
	*/
	GLuint texture_id_;

	/*
	* @brief
	* The type of storage we use for the internal representation of
	* this texture from the choices of Byte, Short, Int, Float.
	* (In this project, we will use float most often).
	*/
	StorageType storage_type_;
	GLenum gl_storage_type_;

	/*
	* @brief
	* The number of channels the texture has. This will be
	* R for single channel / greyscale textures, RG for two channel textures,
	* RGB for generic color data, and RGBA for the default.
	*
	* We can just use RGBA for most cases and switch when we start to
	* care about the memory footprint of the data.
	*/
	ChannelType channel_type_;
	GLenum gl_channel_type_;

	/*
	* @brief
	* The dimensions / number of pixels for this texture. This MUST be a power of 2
	* for OpenGL to cooperate nicely. (GPUs do not like data that is not uniform).
	*/
	glm::ivec2 dimensions_;

	/*
	* @brief
	* Tracks if this texture is valid, which it might not be if invalid parameters to the constructor are passed in.
	* 
	* TODO: This might not be entirely necessary since we can track validity by seeing if the texture_id_ != 0
	*/
	bool valid_texture_;
	
	void GenTexture(const void* data = nullptr);

	// For children classes so that they don't have to follow the same format as the public facing API
	Texture2D() : dimensions_(glm::ivec2(0, 0)), storage_type_(StorageType::TEX_FLOAT), 
		channel_type_(ChannelType::RGBA), gl_storage_type_(GL_NONE), gl_channel_type_(GL_NONE), 
		valid_texture_(false), texture_id_(0) {};
public:
	Texture2D(glm::ivec2 dimensions, StorageType storage_type = StorageType::TEX_FLOAT, 
		ChannelType channel_type = ChannelType::RGBA, const void* initial_data = nullptr);

	/*
	* Loads in a texture with the given filename.
	* The texture is assumed to be in "resources/textures/"
	* and the call will fail if the texture is located elsewhere.
	* 
	* If the texture is in a subdirectory in texture, pass in
	* as "subdirectory/texture.png"
	* 
	* @param
	* filename:
	* The name of the file. The path is assumed and thus
	* the texture must be in "resources/textures/"
	*/
	Texture2D(const std::string& texture_filename);

	~Texture2D();
	/*
	* Returns the texture ID associated with this texture.
	* Useful for binding a texture to the FrameBuffer or
	* performing other modifications to the texture.
	*
	* Example of rendering to texture:
	* > // Set "renderedTexture" as our colour attachement #0
	* > glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Texture.GetTextureId(), 0);
	*
	*/
	GLuint GetTextureId() const;

	bool ModifyTextureData(glm::ivec2 top_left_start, glm::ivec2 data_dimensions, const void* texture_data);
	void SetNewData(glm::ivec2 dimensions, const void* data);
	glm::ivec2 GetDimensions() const;
	GLenum GetGLStorageType() const;
	GLenum GetGLChannelType() const;

	/*
	* @brief
	* Used to bind the texture to some texture unit for use in an upcoming shader / render call
	*/
	virtual bool ActiveBind(GLenum texture_unit_binding);
	/*
	* @brief
	* Used to bind the texture to an image texture unit for use in a shader. Allows for writes in a compute shader and
	* binding for rendering to texture.
	*/
	virtual bool BindImageTexture(GLenum texture_unit_binding);
};

class Texture3D {
private:
protected:

	/*
	* @brief
	* Tracks the texture created on the GPU with a texture id
	* Manipulating the texture on the GPU will require this id
	*/
	GLuint texture_id_;

	/*
	* @brief
	* The type of storage we use for the internal representation of
	* this texture from the choices of Byte, Short, Int, Float.
	* (In this project, we will use float most often).
	*/
	StorageType storage_type_;
	GLenum gl_storage_type_;

	/*
	* @brief
	* The number of channels the texture has. This will be
	* R for single channel / greyscale textures, RG for two channel textures,
	* RGB for generic color data, and RGBA for the default.
	* 
	* We can just use RGBA for most cases and switch when we start to
	* care about the memory footprint of the data.
	*/
	ChannelType channel_type_;
	GLenum gl_channel_type_;

	/*
	* @brief
	* The dimensions in x, y, z for this 3D texture.
	*/
	glm::ivec3 dimensions_;

	/*
	* @brief
	* Tracks if this texture is valid, which it might not be if invalid parameters to the constructor are passed in.
	* 
	* TODO: This might not be entirely necessary since we can track validity by seeing if the texture_id_ != 0
	*/
	bool valid_texture_;

	void GenTexture(const void* data);

public:
	Texture3D(glm::ivec3 dimensions, StorageType storage_type = StorageType::TEX_FLOAT, 
		ChannelType channel_type = ChannelType::RGBA, const void* initial_data = nullptr);

	~Texture3D();

	/*
	* Returns the texture ID associated with this texture.
	* Useful for binding a texture to the FrameBuffer or
	* performing other modifications to the texture.
	* 
	* Example of rendering to texture:
	* > // Set "renderedTexture" as our colour attachement #0
	* > glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Texture.GetTextureId(), 0);
	* 
	*/
	GLuint GetTextureId() const;

	bool ModifyTextureData(glm::ivec3 top_left_start, glm::ivec3 data_dimensions, const void* texture_data);
	void SetNewData(glm::ivec3 dimensions, const void* texture_data);
	glm::ivec3 GetDimensions() const;
	GLenum GetGLStorageType() const;
	GLenum GetGLChannelType() const;

	/*
	* @brief
	* Used to bind the texture to some texture unit for use in an upcoming shader / render call
	*/
	virtual bool ActiveBind(GLenum texture_unit_binding);
	/*
	* @brief
	* Used to bind the texture to an image texture unit for use in a shader. Allows for writes in a compute shader and
	* binding for rendering to texture. 
	*/
	virtual bool BindImageTexture(GLenum texture_unit_binding);
};

/*
* @brief
* A class that is fairly similar to the texture class,
* but instead manages the boilerplate for creating cubemaps.
*
*/
class Cubemap : public Texture2D {
public:
	/*
	* @brief
	* Loads in images given in the vector of string paths
	* for each face of the cubemap. The given files are
	* assumed to be in the order of:
	*		- "right" or positive X
	*		- "left" or negative X
	*		- "top" or positive Y
	*		- "bottom" or negative Y
	*		- "front" or positive Z
	*		- "back" or negative Z
	*
	* @param
	* skybox:
	* An optional parameter to set whether this cubemap is a skybox
	* or not. Default is true
	*/
	Cubemap(std::vector<std::string> input_textures);
	~Cubemap();

	virtual bool ActiveBind(GLenum texture_unit_binding);
};

#endif