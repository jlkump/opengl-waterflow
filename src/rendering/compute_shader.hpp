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
	* A barrier to prevent other threads on the GPU from getting ahead on computation. (or
	* perhaps to prevent the CPU from continuing) either way, its important for water flow
	* simulation to update in steps.
	*/
	void Barrier();

	/*
	* Has the compute shader execute on the GPU when called. This is called before
	* calling Barrier. Barrier waits for execution to be complete.
	*/
	void Dispatch();
};