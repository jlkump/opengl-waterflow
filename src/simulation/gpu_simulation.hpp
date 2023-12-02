#ifndef GPU_SIM_H
#define GPU_SIM_H

#include "sequential_simulation.hpp"
#include "rendering/compute_shader.hpp"

class GPU_Simulation : public Simulation {
private:
	ComputeShader copy_new_to_old_shader_;
	ComputeShader init_grid_shader_;
	ComputeShader move_particles_shader_;
	ComputeShader particle_to_grid_shader_;
	ComputeShader average_grid_shader_;
	ComputeShader grid_incompressability_shader_;
	ComputeShader grid_to_particle_shader_;

	Texture3D grid_vel_x;
	Texture3D grid_vel_y;
	Texture3D grid_vel_z;

	Texture3D grid_old_vel_x;
	Texture3D grid_old_vel_y;
	Texture3D grid_old_vel_z;

	Texture3D grid_count_x;
	Texture3D grid_count_y;
	Texture3D grid_count_z;

	Texture3D grid_is_fluid;
	Texture3D grid_cell_type;


	Texture2D particle_pos_x;
	Texture2D particle_pos_y;
	Texture2D particle_pos_z;

	Texture2D particle_vel_x;
	Texture2D particle_vel_y;
	Texture2D particle_vel_z;

	Texture3D* old_x_ = &grid_old_vel_x;
	Texture3D* old_y_ = &grid_old_vel_y;
	Texture3D* old_z_ = &grid_old_vel_z;

	Texture3D* new_x_ = &grid_vel_x;
	Texture3D* new_y_ = &grid_vel_y;
	Texture3D* new_z_ = &grid_vel_z;

	unsigned int grid_dim_;
	float ws_grid_interval_;
	glm::vec3 ws_lower_bound_grid_;
	glm::vec3 ws_lower_bound_particles_;
	glm::vec3 ws_upper_bound_grid_;
	glm::vec3 ws_upper_bound_particles_;

	const float k_texture_precision_;
	int iterations_;
	float flip_ratio_;
public:
	GPU_Simulation(int num_particles_sqrt, int grid_dim, int iteration);
	~GPU_Simulation();

	virtual void SetInitialVelocities(const std::vector<glm::vec3>& initial, glm::vec3 lower_bound, glm::vec3 upper_bound, float interval);
	virtual void TimeStep(float delta);
	virtual std::vector<glm::vec3>* GetGridVelocities();
	virtual unsigned int GetGridDimensions();
	virtual glm::vec3 GetGridUpperBounds();
	virtual glm::vec3 GetGridLowerBounds();
	virtual float GetGrindInterval();
	virtual std::vector<float>* GetGridPressures();
	virtual std::vector<float>* GetGridDyeDensities();
	virtual std::vector<float>* GetGridFluidCells();
	virtual std::vector<glm::vec3>* GetParticleVelocities();
	virtual std::vector<glm::vec3>* GetParticlePositions();

	Texture2D* GetTexParticlePositions_X();
	Texture2D* GetTexParticlePositions_Y();
	Texture2D* GetTexParticlePositions_Z();

	float GetTexturePrecision();
};


#endif // !GPU_SIM_H
