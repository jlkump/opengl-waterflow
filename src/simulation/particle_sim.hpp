#ifndef PARTICLE_SIM_C
#define PARTICLE_SIM_C

#include <vector>
#include <glm/glm.hpp>

struct SSBO {
	// Values for the SSBO of the particle sim
	float delta_time_;
	std::vector<glm::vec3> positions_;
	std::vector<glm::vec3> velocities_;
	std::vector<glm::vec3> quad_vertices_;
	std::vector<bool> solid_cells_; // True when solid
};

class ParticleSim {
private:
	SSBO ssbo_data_;

	std::vector<int> indices;

public:

	/*
	* @brief
	* Returns a pointer to the start of the ssbo data for the compute shader.
	* Modifies data_bytes to have the length of the ssbo in bytes.
	* 
	* @param
	* data_bytes:
	* Modified with the length of the ssbo data's length in bytes
	*/
	const void* GetSSBOData(unsigned int& data_bytes);

	/*
	* Information from this call is passed to the particle renderer
	*/
	const std::vector<glm::vec3>& GetParticleSpriteQuadVertices();
};

class ParticleRenderer {

	const std::vector<glm::vec3>& particle_sprite_quads_;
	const std::vector<glm::vec3> particle_sprite_indices_;
};

#endif // !PARTICLE_GRID_C