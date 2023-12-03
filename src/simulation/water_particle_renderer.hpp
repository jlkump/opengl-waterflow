#ifndef PARTICLE_RENDERER_C
#define PARTICLE_RENDERER_C

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "../rendering/texture.hpp"
#include "../rendering/shader.hpp"
#include "../rendering/camera.hpp"
#include "../rendering/skybox.hpp"

#define MAX_NUM_PARTICLES 512 * 512

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
class WaterParticleRenderer {
private:
	std::vector<glm::vec3> kQuadData_ = {
		{ -1.0f, -1.0f, 0.0f},
		{ 1.0f, -1.0f, 0.0f},
		{ -1.0f, 1.0f, 0.0f},
		{ 1.0f, 1.0f, 0.0f},
	};

	std::vector<unsigned short> kQuadIndices_ = {
		0, 1, 2, 2, 1, 3
	};

	int viewport_width_ = 1024;
	int viewport_height_ = 768;
	int reduce_resolution_factor_ = 2;

	///////////////////////////////
	// Particle Sprite Rendering //
	///////////////////////////////
	void InitializeParticleRenderingVariables();
	GLuint particle_VAO_;
	GLuint particle_billboard_buffer_;
	GLuint particle_index_buffer_;
	Shader particle_shader_;
	int particle_count_ = 0;
	float particle_radius_ = 0.1f;
	// We render depth to this image, nothing gets rendered to the screen on the first pass.
	GLuint particle_frame_buffer_id_;
	Texture2D depth_texture_;
	
	// We will be rendering using textures rather than real model vertex data, 
	// so use a simple screen-space quad for future passes
	void InitializeScreenQuadVariables();
	GLuint quad_VAO_;
	GLuint quad_position_buffer_;
	GLuint quad_index_buffer_;
	void DrawParticleSprites(glm::mat4& view_mat, glm::mat4& proj_mat);


	//////////////////////
	// Smoothing Shader //
	//////////////////////
	// We now smooth the rendered textures
	void InitializeSmoothingVariables();
	Shader smoothing_shader_;
	GLuint smoothing_frame_buffer_id_;
	Texture2D smoothed_depth_texture_;
	void SmoothDepthTexture();

	//////////////////
	// Water Shader //
	//////////////////
	Shader water_shader_;
	void DrawWater(glm::mat4& inv_view, glm::mat4& inv_proj, glm::vec3& cam_pos, glm::vec3& light_dir, Skybox& skybox);

	// TODO: Add references to the scene elements (camera, skybox) and only update uniforms when they change
	// Will be far more efficient than updating on every draw frame

public:
	WaterParticleRenderer();

	void UpdateParticlePositionsTexture(Texture2D& positions_x, Texture2D& positions_y, Texture2D& positions_z);

	void Draw(Camera& cam, Skybox& skybox);

};

#endif // !PARTICLE_GRID_C