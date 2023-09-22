#ifndef PARTICLE_SIM_C
#define PARTICLE_SIM_C

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "../rendering/Shader.hpp"
#include "../rendering/Camera.hpp"


#define NUM_PARTICLES 512

struct SSBO {
	// Values for the SSBO of the particle sim
	float delta_time_;
	std::vector<glm::vec3> positions_;
	std::vector<glm::vec3> velocities_;
	std::vector<bool> solid_cells_; // True when solid
};

class PicFlipRenderer {
private:
	std::vector<glm::vec3> kParticleQuadData_ = {
		{ -0.5f, -0.5f, 0.0f},
		{ 0.5f, -0.5f, 0.0f},
		{ -0.5f, 0.5f, 0.0f},
		{ 0.5f, 0.5f, 0.0f},
	};
	// Found a better solution, use instancing
	GLuint billboard_buffer_;
	GLuint position_buffer_;
	Shader particle_shader_;

	int particle_count_ = 0;

	float particle_radius_ = 0.1f;

public:
	PicFlipRenderer();

	void UpdateParticlePositions(std::vector<glm::vec3>& positions);

	void Draw(Camera& cam);


};

#endif // !PARTICLE_GRID_C