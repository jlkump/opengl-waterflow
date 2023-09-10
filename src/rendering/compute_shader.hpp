#pragma once

#include "shader.hpp"

class ComputeShader : public Shader {
private:
	/// <summary>
	/// Specific to compute shaders, determines the size of the global work group.
	/// This is used in dispatching calls to the compute shader.
	/// </summary>
	glm::vec3 work_group_dim_;

public:
	/*
	* @brief
	* A specific constuctor for a compute shader, as no other shaders can be run
	* during the execution of this GPU program. (The execution of a compute shader lies
	* outside the GPU pipeline.)
	* 
	* @param
	* compute_shader_file_name: The filename of the associated compute shader code. This is
	* assumed to be inside the resources/shaders/ folder and will fail if the code is elsewhere.
	* 
	* @param
	* work_group_dim: Defines the dimensions of the work group for the compute shader. For
	* a 2D work group, we use (x, y, 1).
	*/
	ComputeShader(const std::string& compute_shader_file_name, const glm::vec3& work_group_dim);

	/*
	* @brief
	* Waits for the execution of the shader code to be complete.
	* 
	* A barrier to prevent other threads on the GPU from getting ahead on computation. (or
	* perhaps to prevent the CPU from continuing). Either way, it allows the
	* computation to update in steps.
	*/
	void Barrier();

	/*
	* @brief
	* Has the compute shader execute on the GPU when called. This is called before
	* calling Barrier() but after calling SetActive().
	*/
	void Dispatch();

	/*
	* @brief
	* Generates and binds an SSBO (Shader Storage Buffer Object) to
	* the compute shader. This is useful for passing
	* aribitrary data to the shader, such as an array of points, velocities, etc.
	* 
	* 
	* Note: Uniforms are still used in compute shaders, 
	* SSBOs are for arbitrary data that is difficult or unwieldy to
	* deal with in the storage types of uniforms
	* 
	* 
	* The data of the SSBO can be updated with calls to UpdateSSBO().
	* 
	* 
	* It is generally better to have one large SSBO with all the data
	* necessary than to have multiple small SSBOs. 
	* 
	* @param
	* data: A ptr to the data contained in the SSBO. This does not need to be an
	* array, it can be a struct. The structure is reflected in the compute shader code.
	* 
	* @param
	* data_size: The amount of data that is contained in the pointer to data, measured in bytes
	* 
	* @param
	* target_binding: The binding to link the SSBO to, reflected in the binding in
	* the compute shader.
	*/
	GLuint GenerateAndBindSSBO(const void* data, unsigned int data_size, GLuint target_binding);

	/*
	* @brief
	* This call is used for updating an existing SSBO's data. The
	* updated data can be a section of the SSBO's full data. The
	* range updated is defined by the starting offset within
	* the data and the size being updated. Size and offset
	* are measured in bytes.
	* 
	* @brief
	* Call GenerateAndBindSSBO() to create an SSBO within the
	* compute shader.
	* 
	* @param
	* ssbo: The ssbo integer handle (returned from the call to GenerateAndBindSSBO().
	* 
	* @param
	* data: A ptr to the updated data.
	* 
	* @param
	* data_size: The size of the data being updated, measured in bytes.
	* 
	* @param
	* data_offset: The offset within the data to start updating at.
	*/
	void UpdateSSBO(GLuint ssbo, const void* data, unsigned int data_size, unsigned int data_offset);

	// Technically, SSBO's are not linked to a specific program, but if my use-case requires shared SSBO data,
	// I will update this class.
};