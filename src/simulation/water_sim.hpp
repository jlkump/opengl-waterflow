#ifndef PARTICLE_SIM_C
#define PARTICLE_SIM_C

#include <vector>
#include <glm/glm.hpp>

struct SSBO {
	// Values for the SSBO of the particle sim
	float delta_time_;
	std::vector<glm::vec3> positions_;
	std::vector<glm::vec3> velocities_;
	std::vector<glm::vec3> quad_vertices_; // 4 quad_vertices per particle
	std::vector<bool> solid_cells_; // True when solid
};

class ParticleGrid {
private:
	SSBO ssbo_data_;

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
	#define NUM_PARTICLES 512 // This must match the number of particles in the pic_flip_shader.comp

	void InitializeParticles(std::vector<glm::vec3>& particle_positions, const glm::vec3& lower_bound, const glm::vec3& upper_bound) {
		static const double delta = 0.125f;
		int i = 0;
		for (double x = lower_bound.x; x < upper_bound.x; x += delta) {
			for (double y = lower_bound.y; y < upper_bound.y; y += delta) {
				for (double z = lower_bound.z; z < upper_bound.z; z += delta) {
					if (i < NUM_PARTICLES) {
						particle_positions[i] = glm::vec3(x, y, z);
						printf("Particle at: %.1f, %.1f, %.1f\n", x, y, z);
					}
					i++;
				}
			}
		}
	}
	*/

	/*
	* Information from this call is passed to the particle renderer
	*/
	const std::vector<glm::vec3>& GetParticleSpriteQuadVertices();
};


class PicFlipRenderer {
private:
	const std::vector<glm::vec3>& particle_sprite_quads_;
	const std::vector<glm::vec3> particle_sprite_indices_;
	// TODO: VAO, VBO data'
	// Do the following in the compute shader (b/c why not? It'll be much faster)
	//  (particle position --> 4 vertices spaced in screen-space spaced acording to plane tangent to particle position and view direction)

public:

	void Draw();


};

#endif // !PARTICLE_GRID_C