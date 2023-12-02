#include "gpu_simulation.hpp"

GPU_Simulation::GPU_Simulation(int num_particles_sqrt, int grid_dimen, int iteration) :
	init_grid_shader_("compute/init_grid.comp", glm::ivec3(grid_dimen + 1, grid_dimen + 1, grid_dimen + 1)),
	move_particles_shader_("compute/move_particles.comp", glm::ivec3(num_particles_sqrt, num_particles_sqrt, 1)),
	particle_to_grid_shader_("compute/particle_to_grid.comp", glm::ivec3(num_particles_sqrt, num_particles_sqrt, 1)),
	average_grid_shader_("compute/average_grid.comp", glm::ivec3(grid_dimen + 1, grid_dimen + 1, grid_dimen + 1)),
	grid_incompressability_shader_("compute/grid_incompressability.comp", glm::ivec3(grid_dimen + 1, grid_dimen + 1, grid_dimen + 1)),
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
	grid_is_fluid(glm::ivec3(grid_dimen + 1), StorageType::TEX_INT, ChannelType::R32UI),
	grid_cell_type(glm::ivec3(grid_dimen + 1), StorageType::TEX_INT, ChannelType::R32UI),
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
	iterations_(iteration)
{
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

}

GPU_Simulation::~GPU_Simulation()
{
}

void GPU_Simulation::SetInitialVelocities(const std::vector<glm::vec3>& initial, glm::vec3 lower_bound, glm::vec3 upper_bound, float interval)
{
	std::vector<int> particle_vel_data_x = std::vector<int>(initial.size());
	std::vector<int> particle_vel_data_y = std::vector<int>(initial.size());
	std::vector<int> particle_vel_data_z = std::vector<int>(initial.size());

	for (int i = 0; i < initial.size(); i++) {
		particle_vel_data_x.push_back(initial[i].x); // todo conversion
	}

	particle_vel_x.SetNewData(glm::ivec2(floor(sqrt(initial.size()))), (const void*)&particle_vel_data_x[0]);
}

void GPU_Simulation::TimeStep(float delta)
{

	init_grid_shader_.SetActive();
	new_x_->ActiveBind(0);
	new_y_->ActiveBind(1);
	new_z_->ActiveBind(2);
	grid_count_x.ActiveBind(3);
	grid_count_y.ActiveBind(4);
	grid_count_z.ActiveBind(5);
	old_x_->ActiveBind(6);
	old_y_->ActiveBind(7);
	old_z_->ActiveBind(8);
	init_grid_shader_.Dispatch();
	init_grid_shader_.Barrier();

	move_particles_shader_.SetUniform1fv("delta_time", delta);
	move_particles_shader_.SetActive();
	particle_pos_x.ActiveBind(0);
	particle_pos_y.ActiveBind(1);
	particle_pos_z.ActiveBind(2);
	particle_vel_x.ActiveBind(3);
	particle_vel_y.ActiveBind(4);
	particle_vel_z.ActiveBind(5);
	move_particles_shader_.Dispatch();
	move_particles_shader_.Barrier();

	particle_to_grid_shader_.SetActive();
	new_x_->ActiveBind(0);
	new_y_->ActiveBind(1);
	new_z_->ActiveBind(2);
	grid_count_x.ActiveBind(3);
	grid_count_y.ActiveBind(4);
	grid_count_z.ActiveBind(5);
	grid_is_fluid.ActiveBind(6);
	grid_cell_type.ActiveBind(7);
	particle_pos_x.ActiveBind(8);
	particle_pos_y.ActiveBind(9);
	particle_pos_z.ActiveBind(10);
	particle_vel_x.ActiveBind(11);
	particle_vel_y.ActiveBind(12);
	particle_vel_z.ActiveBind(13);
	particle_to_grid_shader_.Dispatch();
	particle_to_grid_shader_.Barrier();

	average_grid_shader_.SetActive();
	new_x_->ActiveBind(0);
	new_y_->ActiveBind(1);
	new_z_->ActiveBind(2);
	grid_count_x.ActiveBind(3);
	grid_count_y.ActiveBind(4);
	grid_count_z.ActiveBind(5);
	average_grid_shader_.Dispatch();
	average_grid_shader_.Barrier();



	for (int i = 0; i < iterations_; i++) {
		grid_incompressability_shader_.SetActive();
		new_x_->ActiveBind(0);
		new_y_->ActiveBind(1);
		new_z_->ActiveBind(2);
		old_x_->ActiveBind(3);
		old_y_->ActiveBind(4);
		old_z_->ActiveBind(5);
		grid_is_fluid.ActiveBind(6);
		grid_cell_type.ActiveBind(7);
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

	grid_to_particle_shader_.SetActive();
	new_x_->ActiveBind(0);
	new_y_->ActiveBind(1);
	new_z_->ActiveBind(2);
	grid_is_fluid.ActiveBind(3);
	grid_cell_type.ActiveBind(4);
	particle_pos_x.ActiveBind(5);
	particle_pos_y.ActiveBind(6);
	particle_pos_z.ActiveBind(7);
	particle_vel_x.ActiveBind(8);
	particle_vel_y.ActiveBind(9);
	particle_vel_z.ActiveBind(10);
	old_x_->ActiveBind(11);
	old_y_->ActiveBind(12);
	old_z_->ActiveBind(13);
	grid_to_particle_shader_.Dispatch();
	grid_to_particle_shader_.Barrier();
}

std::vector<glm::vec3>* GPU_Simulation::GetGridVelocities()
{
	return nullptr;
}

unsigned int GPU_Simulation::GetGridDimensions()
{
	return 0;
}

glm::vec3 GPU_Simulation::GetGridUpperBounds()
{
	return glm::vec3();
}

glm::vec3 GPU_Simulation::GetGridLowerBounds()
{
	return glm::vec3();
}

float GPU_Simulation::GetGrindInterval()
{
	return 0.0f;
}

std::vector<float>* GPU_Simulation::GetGridPressures()
{
	return nullptr;
}

std::vector<float>* GPU_Simulation::GetGridDyeDensities()
{
	return nullptr;
}

std::vector<float>* GPU_Simulation::GetGridFluidCells()
{
	return nullptr;
}

std::vector<glm::vec3>* GPU_Simulation::GetParticleVelocities()
{
	return nullptr;
}

std::vector<glm::vec3>* GPU_Simulation::GetParticlePositions()
{
	return nullptr;
}
