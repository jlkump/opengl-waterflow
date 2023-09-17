#ifndef TEXTURE_C
#define TEXTURE_C

#include <string>
#include <glad/glad.h>

class Texture 
{
private:

	/*
	* @brief
	* Tracks the texture created on the GPU
	* which is associated with this Texture object.
	*/
	GLuint texture_obj_id_;

	/*
	* @brief
	* Constants which are used for setting up the texture
	* with calls to glTexImage2d() and similar calls.
	*/
	unsigned int channels_ = 0;
	const GLuint kTextureStorageFormat_[4] = { GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F };
	const GLuint kTextureChannels_[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };

public:
	/*
	* @brief
	* Creates an empty texture which can be useful for drawing
	* to texture with shaders. In this case, the GetTextureId()
	* method will be useful in binding the texture to the
	* frame buffer.
	* 
	* @param
	* desired_channels:
	* The desired number of RGBA channels for the texture.
	*	1 indicates R or a black and white image
	*	2 indicates RG or red green image
	*	3 indicates RGB or colored image
	*	4 indicates RGBA or a colored image with transparency
	* 
	* @ param 
	* dimensions: The dimensions of the texture. Note: This
	* must be a power of 2, otherwise there will be problems
	* with loading the texture into GPU memory.
	*/
	Texture(int desired_channels, int dimensions, const float* data = nullptr);

	/*
	* Loads in a texture with the given filename.
	* The texture is assumed to be in "resources/textures/"
	* and the call will fail if the texture is located elsewhere.
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
	* the texture in the shader program. 
	* Example: GL_TEXTURE0, GL_TEXTURE1, etc.
	* 
	*/
	void ActiveBind(GLint texture_unit);

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