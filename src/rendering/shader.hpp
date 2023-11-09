#ifndef SHADER_C
#define SHADER_C

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <map>
#include <string>

#include "texture.hpp"

class Shader {
protected:
	/// <summary>
	/// A map between the name of a uniform in the programs and the associated
	/// OpenGL index.
	/// </summary>
	std::map<std::string, GLuint> uniform_ids_;

	/// <summary>
	///  The id which identifies the shader programs for OpenGL
	/// </summary>
	GLuint program_id_;

	/// <summary>
	/// Tracks whether the programs have been linked. Will prevent 
	/// the program from running if SetActive() is called with
	/// a failure in linking.
	/// </summary>
	bool is_linked_;

	/*
	* @brief
	* Gets the location of a uniform and stores it in uniform_ids_
	* 
	* @param
	* uniform_name: The name of the uniform, which must match
	* exactly with the name of the uniform in the shader program.
	* 
	* @return
	* returns false when glGetUniformLocation fails to find
	* a uniform with the given uniform_name, usually because
	* names do not match with the shader programs.
	*/
	bool GetUniformLocation(const std::string& uniform_name);

	/*
	* @brief
	* Links the shader programs into one program to be run on the GPU
	* 
	* If there are problems linking, the is_linked_ variable
	* will be set accordingly and prevent SetActive() from
	* being called.
	*/
	void LinkProgram();

	/*
	* @brief
	* Used for loading in the program's text
	* 
	* @param 
	* filename the file to be loaded in as a string. 
	* The path is not passed in, as shaders are
	* assumed to be in the resources/shaders folder.
	* The call will fail if they are not there.
	* 
	* @return
	* Returns the program's text. Returns an empty string when program is empty.
	*/
	std::string LoadFile(const std::string& filename);

	/*
	* @brief
	* A do-nothing constructor for children classes to use and
	* override the constructor
	*/
	Shader() : is_linked_(false), program_id_(0) {};

public:
	/*
	* @brief
	* A helper for the boilerplate of creating
	* and managing shader programs with OpenGL.
	* 
	* The parameters assume that the names are passed
	* in as only the file name, as the path is assumed.
	* (All shaders are located in the path "resources/shaders/"
	*
	* @param
	* vertex_shader_filename:
	* The name of the vertex shader to be loaded in.
	* 
	* @param
	* fragment_shader_filename:
	* The name of the fragment shader to be loaded in.
	*/
	Shader(	const std::string& vertex_shader_file_name,
			const std::string& fragment_shader_file_name );

	~Shader();

	/*
	* @brief
	* Calls glUseProgram to set this shader as the active shader
	* being used for future bindings. Typically called in the
	* render loop.
	* 
	* @return
	* Returns false when the shader has not been linked. True
	* when the program is sucessfully active.
	*/
	bool SetActive();

	/*
	* @brief
	* Sets a uniform texture for use in the shader programs.
	* Note, the name of the uniform must be the same as the name
	* in the vertex and fragment shaders. This can be called again
	* to update the texture actively.
	* 
	* @param
	* uniform_name: The name of the texture uniform, 
	* matching with the name in the fragment and vertex shader.
	* 
	* @param
	* texture: The texture to bind with the shaders.
	* 
	* @param
	* texture_unit: The location to bind the texture to. This
	* must match the location of the texture in the
	* shaders.
	* 
	* @return
	* Returns true if the uniform could be set, false otherwise.
	* (False usually means the uniform_name is incorrect)
	*/
	bool SetUniformTexture2D(const std::string& uniform_name, Texture2D& texture, GLenum texture_unit);

	// Other assorted SetUniforms to be added here as needed
	bool SetUniformMatrix4fv(const std::string& uniform_name, const glm::mat4& matrix);
	bool SetUniformMatrix3fv(const std::string& uniform_name, const glm::mat3& matrix);
	bool SetUniform3fv(const std::string& uniform_name, const glm::vec3& vec);
	bool SetUniform1fv(const std::string& uniform_name, const float& f);
};

#endif