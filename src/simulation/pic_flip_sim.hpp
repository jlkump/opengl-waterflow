#ifndef PARTICLE_SIM_C
#define PARTICLE_SIM_C

#define NUM_PARTICLES 512 * 16

struct SSBO {
	// Values for the SSBO of the particle sim
	float delta_time_;
	std::vector<glm::vec3> positions_;
	std::vector<glm::vec3> velocities_;
	std::vector<bool> solid_cells_; // True when solid
};

class PicFlipSim {

};

#endif