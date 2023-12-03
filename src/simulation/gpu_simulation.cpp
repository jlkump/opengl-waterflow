#include "gpu_simulation.hpp"

GPU_Simulation::GPU_Simulation(int num_particles_sqrt, int grid_dimen, int iteration) :
	copy_new_to_old_shader_("compute/copy_new_to_old.comp", glm::ivec3(grid_dimen + 1)),
	init_grid_shader_("compute/init_grid.comp", glm::ivec3(grid_dimen + 1, grid_dimen + 1, grid_dimen + 1)),
	move_particles_shader_("compute/move_particles.comp", glm::ivec3(num_particles_sqrt, num_particles_sqrt, 1)),
	particle_to_grid_shader_("compute/particle_to_grid.comp", glm::ivec3(num_particles_sqrt, num_particles_sqrt, 1)),
	average_grid_shader_("compute/average_grid.comp", glm::ivec3(grid_dimen, grid_dimen, grid_dimen)),
	grid_incompressability_shader_("compute/grid_incompressability.comp", glm::ivec3(grid_dimen, grid_dimen, grid_dimen)),
	grid_to_particle_shader_("compute/grid_to_particle.comp", glm::ivec3(num_particles_sqrt, num_particles_sqrt, 1)),
	grid_vel_x(glm::ivec3(grid_dimen + 1), StorageType::TEX_INT, ChannelType::R32I), 
	grid_vel_y(glm::ivec3(grid_dimen + 1), StorageType::TEX_INT, ChannelType::R32I), 
	grid_vel_z(glm::ivec3(grid_dimen + 1), StorageType::TEX_INT, ChannelType::R32I),
	grid_old_vel_x(glm::ivec3(grid_dimen + 1), StorageType::TEX_INT, ChannelType::R32I),
	grid_old_vel_y(glm::ivec3(grid_dimen + 1), StorageType::TEX_INT, ChannelType::R32I),
	grid_old_vel_z(glm::ivec3(grid_dimen + 1), StorageType::TEX_INT, ChannelType::R32I), 
	grid_count_x(glm::ivec3(grid_dimen + 1), StorageType::TEX_INT, ChannelType::R32UI),
	grid_count_y(glm::ivec3(grid_dimen + 1), StorageType::TEX_INT, ChannelType::R32UI),
	grid_count_z(glm::ivec3(grid_dimen + 1), StorageType::TEX_INT, ChannelType::R32UI),
	grid_is_fluid(glm::ivec3(grid_dimen), StorageType::TEX_INT, ChannelType::R32UI),
	grid_cell_type(glm::ivec3(grid_dimen), StorageType::TEX_INT, ChannelType::R32UI),
	particle_pos_x(glm::ivec2(num_particles_sqrt, num_particles_sqrt), StorageType::TEX_INT, ChannelType::R32I),
	particle_pos_y(glm::ivec2(num_particles_sqrt, num_particles_sqrt), StorageType::TEX_INT, ChannelType::R32I),
	particle_pos_z(glm::ivec2(num_particles_sqrt, num_particles_sqrt), StorageType::TEX_INT, ChannelType::R32I),
	particle_vel_x(glm::ivec2(num_particles_sqrt, num_particles_sqrt), StorageType::TEX_INT, ChannelType::R32I),
	particle_vel_y(glm::ivec2(num_particles_sqrt, num_particles_sqrt), StorageType::TEX_INT, ChannelType::R32I),
	particle_vel_z(glm::ivec2(num_particles_sqrt, num_particles_sqrt), StorageType::TEX_INT, ChannelType::R32I),
	grid_dim_(grid_dimen),
	ws_grid_interval_(0.5f),
	ws_lower_bound_grid_(glm::vec3(-1, -1, -1)),
	ws_upper_bound_grid_(glm::vec3(1, 1, 1)),
	ws_lower_bound_particles_(glm::vec3(-1, -1, -1)),
	ws_upper_bound_particles_(glm::vec3(1, 1, 1)),
	k_texture_precision_(1000),
	iterations_(iteration),
	flip_ratio_(0.1)
{
	// Setup the compute shaders
	move_particles_shader_.SetUniform1fv("delta_time", 0.0f);
	move_particles_shader_.SetUniform3fv("force", glm::vec3(0, -9.8, 0));
	move_particles_shader_.SetUniform3fv("ws_lower_bound", ws_lower_bound_particles_);
	move_particles_shader_.SetUniform3fv("ws_upper_bound", ws_upper_bound_particles_);
	move_particles_shader_.SetUniform1fv("texture_precision", k_texture_precision_);

	particle_to_grid_shader_.SetUniform1ui("grid_dim", grid_dim_);
	particle_to_grid_shader_.SetUniform1fv("ws_grid_interval", ws_grid_interval_);
	particle_to_grid_shader_.SetUniform3fv("ws_lower_bound", ws_lower_bound_grid_);
	particle_to_grid_shader_.SetUniform3fv("ws_upper_bound", ws_upper_bound_grid_);
	particle_to_grid_shader_.SetUniform1fv("texture_precision", k_texture_precision_);

	average_grid_shader_.SetUniform1fv("texture_precision", k_texture_precision_);

	grid_incompressability_shader_.SetUniform1fv("texture_precision", k_texture_precision_);

	grid_to_particle_shader_.SetUniform1ui("grid_dim", grid_dim_);
	grid_to_particle_shader_.SetUniform1fv("ws_grid_interval", ws_grid_interval_);
	grid_to_particle_shader_.SetUniform3fv("ws_lower_bound", ws_lower_bound_grid_);
	grid_to_particle_shader_.SetUniform3fv("ws_upper_bound", ws_upper_bound_grid_);
	grid_to_particle_shader_.SetUniform1fv("texture_precision", k_texture_precision_);
	grid_to_particle_shader_.SetUniform1fv("flip_ratio", flip_ratio_);
	
	// Setting grid
	unsigned int grid_dim_cubed = grid_dim_ * grid_dim_ * grid_dim_;
	unsigned int grid_dim_plus_one_cubed = (grid_dim_ + 1) * (grid_dim_ + 1) * (grid_dim_ + 1);

	std::vector<int> zero_data_int(grid_dim_plus_one_cubed); // zero-filled vector
	std::vector<unsigned int> zero_data_uint(grid_dim_plus_one_cubed); // zero-filled vector
	std::vector<unsigned int> grid_is_fluid_data(grid_dim_cubed);
	std::vector<unsigned int> grid_cell_type_data(grid_dim_cubed);
	for (int x = 0; x < grid_dim_; x++) {
		for (int y = 0; y < grid_dim_; y++) {
			for (int z = 0; z < grid_dim_; z++) {
				if (x == 0 || y == 0 || z == 0 || x + 1 == grid_dim_ || y + 1 == grid_dim_ || z + 1 == grid_dim_) {
					grid_is_fluid_data[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = 0;
					grid_cell_type_data[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = SOLID;
				}
				else {
					grid_is_fluid_data[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = 1;
					grid_cell_type_data[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = AIR;
				}
			}
		}
	}

	grid_vel_x.SetNewData(glm::ivec3(grid_dim_ + 1), (const void*)&zero_data_int[0]);
	grid_vel_y.SetNewData(glm::ivec3(grid_dim_ + 1), (const void*)&zero_data_int[0]);
	grid_vel_z.SetNewData(glm::ivec3(grid_dim_ + 1), (const void*)&zero_data_int[0]);
	grid_old_vel_x.SetNewData(glm::ivec3(grid_dim_ + 1), (const void*)&zero_data_int[0]);
	grid_old_vel_y.SetNewData(glm::ivec3(grid_dim_ + 1), (const void*)&zero_data_int[0]);
	grid_old_vel_z.SetNewData(glm::ivec3(grid_dim_ + 1), (const void*)&zero_data_int[0]);
	grid_count_x.SetNewData(glm::ivec3(grid_dim_ + 1), (const void*)&zero_data_uint[0]);
	grid_count_y.SetNewData(glm::ivec3(grid_dim_ + 1), (const void*)&zero_data_uint[0]);
	grid_count_z.SetNewData(glm::ivec3(grid_dim_ + 1), (const void*)&zero_data_uint[0]);
	grid_is_fluid.SetNewData(glm::ivec3(grid_dim_), (const void*)&grid_is_fluid_data[0]);
	grid_cell_type.SetNewData(glm::ivec3(grid_dim_), (const void*)&grid_cell_type_data[0]);
}

GPU_Simulation::~GPU_Simulation()
{
}

void GPU_Simulation::SetInitialVelocities(const std::vector<glm::vec3>& initial, glm::vec3 lower_bound, glm::vec3 upper_bound, float interval)
{
	// Setting velocities
	std::vector<int> particle_vel_data_x(initial.size());
	std::vector<int> particle_vel_data_y(initial.size());
	std::vector<int> particle_vel_data_z(initial.size());

	for (const glm::vec3& vel : initial) {
		particle_vel_data_x.push_back((int)(vel.x * k_texture_precision_));
		particle_vel_data_y.push_back((int)(vel.y * k_texture_precision_));
		particle_vel_data_z.push_back((int)(vel.z * k_texture_precision_));
	}

	particle_vel_x.SetNewData(glm::ivec2(floor(sqrt(initial.size()))), (const void*)&particle_vel_data_x[0]);
	particle_vel_y.SetNewData(glm::ivec2(floor(sqrt(initial.size()))), (const void*)&particle_vel_data_y[0]);
	particle_vel_z.SetNewData(glm::ivec2(floor(sqrt(initial.size()))), (const void*)&particle_vel_data_z[0]);



	// Creating positions (currently spreads it evenly across the grid)
	// TODO: under the assumption that all sides are equal
	std::vector<glm::vec3> particle_pos(initial.size());
	float particles_per_side = floor(cbrt(initial.size()));
	int particles_per_side_squared = particles_per_side * particles_per_side;

	int index = 0;
	glm::vec3 grid_interval_magnitude(std::abs(ws_grid_interval_));
	glm::vec3 adjusted_lower = lower_bound + grid_interval_magnitude;
	glm::vec3 adjusted_upper = upper_bound - grid_interval_magnitude;
	float delta = (std::fabs(adjusted_lower.x) + std::fabs(adjusted_upper.x)) / particles_per_side;

	for (float z = adjusted_lower.z; z < adjusted_upper.z && index < particle_pos.size(); z += delta) {
		for (float y = adjusted_lower.y; y < adjusted_upper.y && index < particle_pos.size(); y += delta) {
			for (float x = adjusted_lower.x; x < adjusted_upper.x && index < particle_pos.size(); x += delta) {
				particle_pos[index] = glm::vec3(x, y, z);
				++index;
				//printf("Particle pos at [%f, %f, %f]\n", x, y, z);
			}
		}
	}

	// Setting positions
	std::vector<int> particle_pos_data_x(initial.size());
	std::vector<int> particle_pos_data_y(initial.size());
	std::vector<int> particle_pos_data_z(initial.size());

	for (const glm::vec3& pos : particle_pos) {
		particle_pos_data_x.push_back((int)(pos.x * k_texture_precision_));
		particle_pos_data_y.push_back((int)(pos.y * k_texture_precision_));
		particle_pos_data_z.push_back((int)(pos.z * k_texture_precision_));
		//printf("Pushing particle pos of [%d %d %d]\n", (int)(pos.x * k_texture_precision_), (int)(pos.y * k_texture_precision_), (int)(pos.z * k_texture_precision_));
	}

	particle_pos_x.SetNewData(glm::ivec2(floor(sqrt(initial.size()))), (const void*)&particle_pos_data_x[0]);
	particle_pos_y.SetNewData(glm::ivec2(floor(sqrt(initial.size()))), (const void*)&particle_pos_data_y[0]);
	particle_pos_z.SetNewData(glm::ivec2(floor(sqrt(initial.size()))), (const void*)&particle_pos_data_z[0]);
	//printf("Tex dim are %d %d\n", glm::ivec2(floor(sqrt(initial.size()))).x, glm::ivec2(floor(sqrt(initial.size()))).y);
}

void GPU_Simulation::TimeStep(float delta)
{
	// init_grid
	init_grid_shader_.SetActive();
	new_x_->ActiveBind(GL_TEXTURE0);
	new_y_->ActiveBind(GL_TEXTURE1);
	new_z_->ActiveBind(GL_TEXTURE2);
	grid_count_x.ActiveBind(GL_TEXTURE3);
	grid_count_y.ActiveBind(GL_TEXTURE4);
	grid_count_z.ActiveBind(GL_TEXTURE5);
	init_grid_shader_.Dispatch();
	init_grid_shader_.Barrier();

	// init_particles
	move_particles_shader_.SetUniform1fv("delta_time", delta);
	move_particles_shader_.SetActive();
	particle_pos_x.ActiveBind(GL_TEXTURE0);
	particle_pos_y.ActiveBind(GL_TEXTURE1);
	particle_pos_z.ActiveBind(GL_TEXTURE2);
	particle_vel_x.ActiveBind(GL_TEXTURE3);
	particle_vel_y.ActiveBind(GL_TEXTURE4);
	particle_vel_z.ActiveBind(GL_TEXTURE5);
	move_particles_shader_.Dispatch();
	move_particles_shader_.Barrier();

	// particle_to_grid
	particle_to_grid_shader_.SetUniformTexture2D("particle_positions_x", particle_pos_x, GL_TEXTURE8);
	particle_to_grid_shader_.SetUniformTexture2D("particle_positions_y", particle_pos_y, GL_TEXTURE9);
	particle_to_grid_shader_.SetUniformTexture2D("particle_positions_z", particle_pos_z, GL_TEXTURE10);
	particle_to_grid_shader_.SetUniformTexture2D("particle_velocities_x", particle_vel_x, GL_TEXTURE11);
	particle_to_grid_shader_.SetUniformTexture2D("particle_velocities_y", particle_vel_y, GL_TEXTURE12);
	particle_to_grid_shader_.SetUniformTexture2D("particle_velocities_z", particle_vel_z, GL_TEXTURE13);
	particle_to_grid_shader_.SetActive();
	new_x_->ActiveBind(GL_TEXTURE0);
	new_y_->ActiveBind(GL_TEXTURE1);
	new_z_->ActiveBind(GL_TEXTURE2);
	grid_count_x.ActiveBind(GL_TEXTURE3);
	grid_count_y.ActiveBind(GL_TEXTURE4);
	grid_count_z.ActiveBind(GL_TEXTURE5);
	grid_is_fluid.ActiveBind(GL_TEXTURE6);
	grid_cell_type.ActiveBind(GL_TEXTURE7);
	particle_to_grid_shader_.Dispatch();
	particle_to_grid_shader_.Barrier();

	// average_grid
	average_grid_shader_.SetActive();
	new_x_->ActiveBind(GL_TEXTURE0);
	new_y_->ActiveBind(GL_TEXTURE1);
	new_z_->ActiveBind(GL_TEXTURE2);
	grid_count_x.ActiveBind(GL_TEXTURE3);
	grid_count_y.ActiveBind(GL_TEXTURE4);
	grid_count_z.ActiveBind(GL_TEXTURE5);
	average_grid_shader_.Dispatch();
	average_grid_shader_.Barrier();


	// grid_incompressability
	for (int i = 0; i < iterations_; i++) {
		grid_incompressability_shader_.SetActive();
		new_x_->ActiveBind(GL_TEXTURE0);
		new_y_->ActiveBind(GL_TEXTURE1);
		new_z_->ActiveBind(GL_TEXTURE2);
		old_x_->ActiveBind(GL_TEXTURE3);
		old_y_->ActiveBind(GL_TEXTURE4);
		old_z_->ActiveBind(GL_TEXTURE5);
		grid_is_fluid.ActiveBind(GL_TEXTURE6);
		grid_cell_type.ActiveBind(GL_TEXTURE7);
		grid_incompressability_shader_.Dispatch();
		grid_incompressability_shader_.Barrier();
		Texture3D* temp_x = new_x_;
		Texture3D* temp_y = new_y_;
		Texture3D* temp_z = new_z_;
		new_x_ = old_x_;
		new_y_ = old_y_;
		new_z_ = old_z_;
		old_x_ = temp_x;
		old_y_ = temp_y;
		old_z_ = temp_z;
	}

	// grid_to_particle
	grid_to_particle_shader_.SetUniformTexture3D("grid_velocities_x", *new_x_, GL_TEXTURE8);
	grid_to_particle_shader_.SetUniformTexture3D("grid_velocities_y", *new_y_, GL_TEXTURE9);
	grid_to_particle_shader_.SetUniformTexture3D("grid_velocities_z", *new_z_, GL_TEXTURE10);
	grid_to_particle_shader_.SetUniformTexture3D("grid_old_velocities_x", *old_x_, GL_TEXTURE11);
	grid_to_particle_shader_.SetUniformTexture3D("grid_old_velocities_y", *old_y_, GL_TEXTURE12);
	grid_to_particle_shader_.SetUniformTexture3D("grid_old_velocities_z", *old_z_, GL_TEXTURE13);
	grid_to_particle_shader_.SetActive();
	grid_is_fluid.ActiveBind(GL_TEXTURE0);
	grid_cell_type.ActiveBind(GL_TEXTURE1);
	particle_pos_x.ActiveBind(GL_TEXTURE2);
	particle_pos_y.ActiveBind(GL_TEXTURE3);
	particle_pos_z.ActiveBind(GL_TEXTURE4);
	particle_vel_x.ActiveBind(GL_TEXTURE5);
	particle_vel_y.ActiveBind(GL_TEXTURE6);
	particle_vel_z.ActiveBind(GL_TEXTURE7);
	grid_to_particle_shader_.Dispatch();
	grid_to_particle_shader_.Barrier();

	// copy_new_to_old
	copy_new_to_old_shader_.SetActive();
	new_x_->ActiveBind(0);
	new_y_->ActiveBind(1);
	new_z_->ActiveBind(2);
	old_x_->ActiveBind(3);
	old_y_->ActiveBind(4);
	old_z_->ActiveBind(5);
	copy_new_to_old_shader_.Dispatch();
	copy_new_to_old_shader_.Barrier();
}

std::vector<glm::vec3>* GPU_Simulation::GetGridVelocities()
{
	printf("GPU_Simulation::GetGridVelocities() not implemented yet.\n");
	return nullptr;
}

unsigned int GPU_Simulation::GetGridDimensions()
{
	return grid_dim_;
}

glm::vec3 GPU_Simulation::GetGridUpperBounds()
{
	return ws_upper_bound_grid_;
}

glm::vec3 GPU_Simulation::GetGridLowerBounds()
{
	return ws_lower_bound_grid_;
}

float GPU_Simulation::GetGridInterval()
{
	return ws_grid_interval_;
}

std::vector<float>* GPU_Simulation::GetGridPressures()
{
	printf("GPU_Simulation::GetGridPressures() not implemented yet.\n");
	return nullptr;
}

std::vector<float>* GPU_Simulation::GetGridDyeDensities()
{
	printf("GPU_Simulation::GetGridDyeDensities() not implemented yet.\n");
	return nullptr;
}

std::vector<float>* GPU_Simulation::GetGridFluidCells()
{
	printf("GPU_Simulation::GetGridFluidCells() not implemented yet.\n");
	return nullptr;
}

std::vector<glm::vec3>* GPU_Simulation::GetParticleVelocities()
{
	printf("GPU_Simulation::GetParticleVelocities() not implemented yet.\n");
	return nullptr;
}

std::vector<glm::vec3>* GPU_Simulation::GetParticlePositions()
{
	printf("GPU_Simulation::GetParticlePositions() not implemented yet.\n");
	return nullptr;
}

Texture2D* GPU_Simulation::GetTexParticlePositions_X()
{
	return &particle_pos_x;
}

Texture2D* GPU_Simulation::GetTexParticlePositions_Y()
{
	return &particle_pos_y;
}

Texture2D* GPU_Simulation::GetTexParticlePositions_Z()
{
	return &particle_pos_z;
}

float GPU_Simulation::GetTexturePrecision()
{
	return k_texture_precision_;
}

void GPU_Simulation::Draw()
{
	// TODO: do somehow
}
