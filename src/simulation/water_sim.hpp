#ifndef PARTICLE_SIM_C
#define PARTICLE_SIM_C

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "../rendering/Shader.hpp"
#include "../rendering/Camera.hpp"


#define NUM_PARTICLES 512 * 16

struct SSBO {
	// Values for the SSBO of the particle sim
	float delta_time_;
	std::vector<glm::vec3> positions_;
	std::vector<glm::vec3> velocities_;
	std::vector<bool> solid_cells_; // True when solid
};

/*
* 
* The pic flip renderer uses the following steps in the rendering process.
*	1. Render each particle position as a sprite of a certain radius.
*	   Output this particle rendering's depth to a texture.
* 
*	2. Smooth the depth texture using bilaterial filtering, (to have edges hold better).
* 
*	3. Find the normal from the depth texture and render the water accordingly from there.
* 
*/
class PicFlipRenderer {
private:
	std::vector<glm::vec3> kQuadData_ = {
		{ -1.0f, -1.0f, 0.0f},
		{ 1.0f, -1.0f, 0.0f},
		{ -1.0f, 1.0f, 0.0f},
		{ 1.0f, 1.0f, 0.0f},
	};

	std::vector<unsigned int> kQuadIndices_ = {
		0, 1, 2, 2, 1, 3
	};

	///////////////////////////////
	// Particle Sprite Rendering //
	///////////////////////////////
	void InitializeParticleRenderingVariables();
	GLuint particle_VAO_;
	GLuint particle_billboard_buffer_;
	GLuint particle_position_buffer_;
	Shader particle_shader_;
	int particle_count_ = 0;
	float particle_radius_ = 0.1f;
	// We render depth to this image, nothing gets rendered to the screen on the first pass.
	GLuint frame_buffer_id_;
	GLuint render_texture_id_;
	Texture depth_texture_;
	
	// We will be rendering using textures rather than real model vertex data, 
	// so use a simple screen-space quad for future passes
	void InitializeScreenQuadVariables();
	GLuint quad_VAO_;
	GLuint quad_position_buffer_;
	GLuint quad_index_buffer_;


	//////////////////////
	// Smoothing Shader //
	//////////////////////
	// We now smooth the rendered textures
	Shader smoothing_shader_;
	Texture smoothed_depth_texture_;

	//////////////////
	// Water Shader //
	//////////////////
	Shader water_shader_;

	void DrawParticlesToTexture(glm::mat4& view_mat, glm::mat4& proj_mat);


public:
	PicFlipRenderer();

	void UpdateParticlePositions(std::vector<glm::vec3>& positions);

	void Draw(Camera& cam);


};

#endif // !PARTICLE_GRID_C