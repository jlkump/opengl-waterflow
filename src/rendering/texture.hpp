#ifndef TEXTURE_C
#define TEXTURE_C

#include <string>
#include <glad/glad.h>

class Texture 
{
private:
	/*
	* @brief
	* Tracks the texture created on the GPU with a texture id
	* Manipulating the texture on the GPU will require this id
	*/
	GLuint texture_obj_id_;

	/*
	* @brief
	* The number of color channels for this texture,
	* ranging from 1 to 4 (R, RG, RGB, RGBA)
	*/
	GLenum channels_;

	/*
	* @brief
	* The storage type for this texture, whether it be 0-255 int rgbs or float 0-1.0
	*/
	GLenum storage_type_;

public:
	/*
	* @brief
	* Texture constants for channels and texture storage types
	*/
	//static const GLenum RED = GL_RED;
	//static const GLenum RG = GL_RG;
	//static const GLenum RGB = GL_RGB;
	//static const GLenum RGBA = GL_RGBA;

	//static const GLenum R_INT = GL_R8;
	//static const GLenum RG_INT = GL_RG8;
	//static const GLenum RGB_INT = GL_RGB8;
	//static const GLenum RGBA_INT = GL_RGBA8;

	//static const GLenum R_FLOAT = GL_R32F;
	//static const GLenum RG_FLOAT = GL_RG32F;
	//static const GLenum RGB_FLOAT = GL_RGB32F;
	//static const GLenum RGBA_FLOAT = GL_RGBA32F;


	/*
	* @brief
	* Creates an empty texture which can be useful for drawing
	* to texture with shaders. In this case, the GetTextureId()
	* method will be useful in binding the texture to the
	* frame buffer.
	* 
	* 
	* @ param 
	* dimensions: The dimensions of the texture. Note: This
	* must be a power of 2, otherwise there will be problems
	* with loading the texture into GPU memory.
	* 
	* @param
	* desired_channels:
	* The desired number of RGBA channels for the texture. Defaults to RGBA
	* Specified using: GL_RED, GL_RG, GL_RGB, or GL_RGBA
	* 
	* @param
	* storage type:
	* The way the color for this texture is represented, either as a float or as a byte (0 to 255).
	* Specified using: GL_R32F, ... GL_RGBA32F or GL_R8 ... GL_RGBA8
	* 
	* @param
	* data:
	* The starting data for this texture to be loaded with. Must match the type of the given storage type.
	* Ex: A choice of RGBA32F (RGBA as floats) must have data of [0.2, 0.1, 0.9, 1.0, ...] with each set of
	* four floats representing a color.
	*/
	Texture(int dimensions, GLenum desired_channels = GL_RGBA, GLenum storage_type = GL_RGBA32F, const float* data = nullptr);

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
	Texture(const std::string& filename);
	~Texture();

	/*
	* Calls glActiveTexture and glBindTexture in sequence,
	* which will set this texture as the active
	* texture for future texture based OpenGL calls.
	* Typically called in the for-loop for rendering.
	* 
	* @param
	* texture_unit: A texture_unit must be provided which is
	* associated with the location where to bind
	* the texture in the shader program. Default = GL_TEXTURE0
	* Example: GL_TEXTURE0, GL_TEXTURE1, etc.
	* 
	* 
	*/
	void ActiveBind(GLenum texture_unit = GL_TEXTURE0);

	void BindImage(unsigned int binding);

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
	GLuint GetTextureId();

	void UpdatePixelData(GLint x_offset, GLint y_offset, GLsizei width, GLsizei height, const void* pixel_data);
};

#endif