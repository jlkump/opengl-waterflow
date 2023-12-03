#include "sequential_simulation.hpp"

#include <math.h>

glm::vec3 GetVelocityFrom3DGridCell(const std::vector<glm::vec3>& grid,
	const unsigned int dim,
	const unsigned int x,
	const unsigned int y,
	const unsigned int z) {

	return grid[(x * dim * dim) + (y * dim) + z];
}

void SetVelocityIn3DGridCell(std::vector<glm::vec3>& grid,
	const unsigned int dim,
	const unsigned int x,
	const unsigned int y,
	const unsigned int z,
	glm::vec3 new_velocity) {

	grid[(x * dim * dim) + (y * dim) + z] = new_velocity;
}

float GetDivergence(const std::vector<glm::vec3>& grid,
	const unsigned int dim,
	const unsigned int x,
	const unsigned int y,
	const unsigned int z) {

	glm::vec3 cell = GetVelocityFrom3DGridCell(grid, dim, x, y, z);
	glm::vec3 cell_x = GetVelocityFrom3DGridCell(grid, dim, x + 1, y, z);
	glm::vec3 cell_y = GetVelocityFrom3DGridCell(grid, dim, x, y + 1, z);
	glm::vec3 cell_z = GetVelocityFrom3DGridCell(grid, dim, x, y, z + 1);

	return cell_x.x - cell.x + cell_y.y - cell.y + cell_z.z - cell.z;
}

float GetFloatValFrom3DGridCell(const std::vector<float>& grid, 
	const unsigned int dim, 
	const unsigned int x, 
	const unsigned int y, 
	const unsigned int z)
{
	return grid[x * dim * dim + y * dim + z];
}

void SequentialGridBased::Integrate(float delta, const glm::vec3& acceleration)
{
	for (glm::vec3& vel : velocities_) {
		vel = vel + acceleration * delta;
	}
}

void SequentialGridBased::SolveIncompressability(float delta)
{
	float cp = density_ * ws_grid_interval_ / delta; // For pressure calc
	for (int iter = 0; iter < number_of_iterations_; iter++) {

		for (int x = 1; x < grid_dim_ - 1; x++) {
			for (int y = 1; y < grid_dim_ - 1; y++) {
				for (int z = 1; z < grid_dim_ - 1; z++) {

					if (cell_types_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] != FLUID) {
						continue;
					}

					// TODO: DEBUG Might want to sum total and check instead
					float s = is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z];
					float s_x_neg = is_fluid_[(x-1) * grid_dim_ * grid_dim_ + y * grid_dim_ + z];
					float s_x_pos = is_fluid_[(x+1) * grid_dim_ * grid_dim_ + y * grid_dim_ + z];
					float s_y_neg = is_fluid_[x * grid_dim_ * grid_dim_ + (y-1) * grid_dim_ + z];
					float s_y_pos = is_fluid_[x * grid_dim_ * grid_dim_ + (y+1) * grid_dim_ + z];
					float s_z_neg = is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + (z-1)];
					float s_z_pos = is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + (z+1)];

					s = s_x_neg + s_x_pos + s_y_neg + s_y_pos + s_z_neg + s_z_pos;
					if (s == 0.0) {
						continue;
					}

					float total_divergence = GetDivergence(velocities_, grid_dim_, x, y, z);
					
					glm::vec3 v_cur = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y, z);
					glm::vec3 v_x_pos = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x + 1, y, z);
					glm::vec3 v_y_pos = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y + 1, z);
					glm::vec3 v_z_pos = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y, z + 1);

					float p = -total_divergence / s;
					pressures_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] += cp * p; // Pressure

					v_cur.x -= s_x_neg * p;
					v_cur.y -= s_y_neg * p;
					v_cur.z -= s_z_neg * p;
					v_x_pos.x += s_x_pos * p;
					v_y_pos.y += s_y_pos * p;
					v_z_pos.z += s_z_pos * p;

					SetVelocityIn3DGridCell(velocities_, grid_dim_, x, y, z, v_cur);
					SetVelocityIn3DGridCell(velocities_, grid_dim_, x + 1, y, z, v_x_pos);
					SetVelocityIn3DGridCell(velocities_, grid_dim_, x, y + 1, z, v_y_pos);
					SetVelocityIn3DGridCell(velocities_, grid_dim_, x, y, z + 1, v_z_pos);
				}
			}
		}
	}
}

void SequentialGridBased::BorderConditionUpdate()
{
	for (int y = 0; y < grid_dim_; y++) {
		for (int z = 0; z < grid_dim_; z++) {
			glm::vec3 v_cur = GetVelocityFrom3DGridCell(velocities_, grid_dim_, 0, y, z);
			glm::vec3 v_x_pos = GetVelocityFrom3DGridCell(velocities_, grid_dim_, 1, y, z);
			v_cur.x = v_x_pos.x;
			SetVelocityIn3DGridCell(velocities_, grid_dim_, 0, y, z, v_cur);

			v_cur = GetVelocityFrom3DGridCell(velocities_, grid_dim_, grid_dim_, y, z);
			glm::vec3 v_x_neg = GetVelocityFrom3DGridCell(velocities_, grid_dim_, grid_dim_ - 1, y, z);
			v_cur.x = v_x_neg.x;
			SetVelocityIn3DGridCell(velocities_, grid_dim_, grid_dim_, y, z, v_cur);
		}
	}
	for (int x = 0; x < grid_dim_; x++) {
		for (int z = 0; z < grid_dim_; z++) {
			glm::vec3 v_cur = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, 0, z);
			glm::vec3 v_y_pos = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, 1, z);
			v_cur.y = v_y_pos.y;
			SetVelocityIn3DGridCell(velocities_, grid_dim_, x, 0, z, v_cur);

			v_cur = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, grid_dim_, z);
			glm::vec3 v_y_neg = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, grid_dim_ -1, z);
			v_cur.y = v_y_neg.y;
			SetVelocityIn3DGridCell(velocities_, grid_dim_, x, grid_dim_, z, v_cur);
		}
	}
	for (int x = 0; x < grid_dim_; x++) {
		for (int y = 0; y < grid_dim_; y++) {
			glm::vec3 v_cur = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y, 0);
			glm::vec3 v_z_pos = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y, 1);
			v_cur.z = v_z_pos.z;
			SetVelocityIn3DGridCell(velocities_, grid_dim_, x, y, 0, v_cur);

			v_cur = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y, grid_dim_);
			glm::vec3 v_z_neg = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y, grid_dim_ - 1);
			v_cur.z = v_z_neg.z;
			SetVelocityIn3DGridCell(velocities_, grid_dim_, x, y, grid_dim_, v_cur);
		}
	}
}

void SequentialGridBased::AdvectVelocity(float delta)
{
	std::vector<glm::vec3> new_velocities = std::vector<glm::vec3>(velocities_.size());

	for (int x = 1; x < grid_dim_; x++) {
		for (int y = 1; y < grid_dim_; y++) {
			for (int z = 1; z < grid_dim_; z++) {
				// Advect the x component for this face of the grid cell
				if (is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] != 0.0f &&
					is_fluid_[(x - 1) * grid_dim_ * grid_dim_ + y * grid_dim_ + z] != 0.0f &&
					y < grid_dim_ - 1 && z < grid_dim_ - 1) {
					glm::vec3 ws_pos = glm::vec3(
						x * ws_grid_interval_,
						y * ws_grid_interval_ + ws_grid_interval_ / 2.0f, 
						z * ws_grid_interval_ + ws_grid_interval_ / 2.0f
					);
					glm::vec3 old = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y, z);
					glm::vec3 v = glm::vec3(
						old.x,
						GetAvgYVel(x, y, z, x - 1, y, z), 
						GetAvgZVel(x, y, z, x - 1, y, z)
					);
					ws_pos = ws_pos + v * -delta;
					v = glm::vec3(SampleGridVelocity(ws_pos, X_VEL), old.y, old.z);
					SetVelocityIn3DGridCell(new_velocities, grid_dim_, x, y, z, v);
				}

				// Advect the y component for this face of the grid cell
				if (is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] != 0.0f &&
					is_fluid_[x * grid_dim_ * grid_dim_ + (y - 1) * grid_dim_ + z] != 0.0f &&
					x < grid_dim_ - 1 && z < grid_dim_ - 1) {
					glm::vec3 ws_pos = glm::vec3(
						x * ws_grid_interval_ + ws_grid_interval_ / 2.0f,
						y * ws_grid_interval_,
						z * ws_grid_interval_ + ws_grid_interval_ / 2.0f
					);
					glm::vec3 old = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y, z);
					glm::vec3 v = glm::vec3(
						GetAvgXVel(x, y, z, x, y - 1, z),
						old.y,
						GetAvgZVel(x, y, z, x, y - 1, z)
					);
					ws_pos = ws_pos + v * -delta;
					v = glm::vec3(old.x, SampleGridVelocity(ws_pos, Y_VEL), old.z);
					SetVelocityIn3DGridCell(new_velocities, grid_dim_, x, y, z, v);
				}

				// Advect the z component for this face of the grid cell
				if (is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] != 0.0f &&
					is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z - 1] != 0.0f &&
					y < grid_dim_ - 1 && x < grid_dim_ - 1) {
					glm::vec3 ws_pos = glm::vec3(
						x * ws_grid_interval_ + ws_grid_interval_ / 2.0f,
						y * ws_grid_interval_ + ws_grid_interval_ / 2.0f,
						z * ws_grid_interval_
					);
					glm::vec3 old = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y, z);
					glm::vec3 v = glm::vec3(
						GetAvgXVel(x, y, z, x - 1, y, z),
						GetAvgYVel(x, y, z, x - 1, y, z),
						old.z
					);
					ws_pos = ws_pos + v * -delta;
					v = glm::vec3(old.x, old.y, SampleGridVelocity(ws_pos, Z_VEL));
					SetVelocityIn3DGridCell(new_velocities, grid_dim_, x, y, z, v);
				}
			}
		}
	}

	velocities_ = new_velocities;
}

float SequentialGridBased::GetAvgXVel(unsigned int x, unsigned int y, unsigned int z, unsigned int x_other, unsigned int y_other, unsigned int z_other)
{
	return (GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y, z).x +
		GetVelocityFrom3DGridCell(velocities_, grid_dim_, x + 1, y, z).x +
		GetVelocityFrom3DGridCell(velocities_, grid_dim_, x_other, y_other, z_other).x +
		GetVelocityFrom3DGridCell(velocities_, grid_dim_, x_other + 1, y_other, z_other).x) * 0.25;
}

float SequentialGridBased::GetAvgYVel(unsigned int x, unsigned int y, unsigned int z, unsigned int x_other, unsigned int y_other, unsigned int z_other)
{
	return (GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y, z).y + 
		GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y + 1, z).y +
		GetVelocityFrom3DGridCell(velocities_, grid_dim_, x_other, y_other, z_other).y +
		GetVelocityFrom3DGridCell(velocities_, grid_dim_, x_other, y_other + 1, z_other).y) * 0.25;
}

float SequentialGridBased::GetAvgZVel(unsigned int x, unsigned int y, unsigned int z, unsigned int x_other, unsigned int y_other, unsigned int z_other)
{
	return (GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y, z).z +
		GetVelocityFrom3DGridCell(velocities_, grid_dim_, x, y, z + 1).z +
		GetVelocityFrom3DGridCell(velocities_, grid_dim_, x_other, y_other, z_other).z +
		GetVelocityFrom3DGridCell(velocities_, grid_dim_, x_other, y_other, z_other + 1).z) * 0.25;
}

float SequentialGridBased::SampleGridVelocity(glm::vec3 ws_pos, SampleType s)
{
	float one_over_ws_interval = 1.0 / ws_grid_interval_;
	float half_ws_interval = ws_grid_interval_ * 0.5;

	ws_pos.x = fmax(fmin(ws_pos.x - ws_lower_bound_.x, ws_upper_bound_.x - ws_lower_bound_.x), ws_grid_interval_);
	ws_pos.y = fmax(fmin(ws_pos.y - ws_lower_bound_.y, ws_upper_bound_.y - ws_lower_bound_.y), ws_grid_interval_);
	ws_pos.z = fmax(fmin(ws_pos.z - ws_lower_bound_.z, ws_upper_bound_.z - ws_lower_bound_.z), ws_grid_interval_);
	
	float dx = 0.0f;
	float dy = 0.0f;
	float dz = 0.0f;

	switch (s) {
	case X_VEL: dy = half_ws_interval;  dz = half_ws_interval;  break;
	case Y_VEL: dx = half_ws_interval;  dz = half_ws_interval; break;
	case Z_VEL: dx = half_ws_interval;  dy = half_ws_interval; break;
	}

	unsigned int x0 = fmin((unsigned int) floor(ws_pos.x - dx * one_over_ws_interval), grid_dim_ - 1);
	float tx = ((ws_pos.x - dx) - ((float)x0) * ws_grid_interval_) * one_over_ws_interval;
	unsigned int x1 = fmin(x0 + 1, grid_dim_ - 1);

	unsigned int y0 = fmin((unsigned int)floor(ws_pos.y - dy * one_over_ws_interval), grid_dim_ - 1);
	float ty = ((ws_pos.y - dy) - ((float)y0) * ws_grid_interval_) * one_over_ws_interval;
	unsigned int y1 = fmin(y0 + 1, grid_dim_ - 1);

	unsigned int z0 = fmin((unsigned int)floor(ws_pos.z - dz * one_over_ws_interval), grid_dim_ - 1);
	float tz = ((ws_pos.z - dz) - ((float)z0) * ws_grid_interval_) * one_over_ws_interval;
	unsigned int z1 = fmin(z0 + 1, grid_dim_ - 1);

	float sx = 1.0f - tx;
	float sy = 1.0f - ty;
	float sz = 1.0f - tz;

	float v_0 = 0.0f;
	float v_1 = 0.0f;
	float v_2 = 0.0f;
	float v_3 = 0.0f;
	float v_4 = 0.0f;
	float v_5 = 0.0f;
	float v_6 = 0.0f;
	float v_7 = 0.0f;
	switch (s) {
	case X_VEL:
		v_0 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y0, z0).x;
		v_1 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y0, z0).x;
		v_2 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y0, z1).x;
		v_3 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y0, z1).x;
																			
		v_4 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y1, z0).x;
		v_5 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y1, z0).x;
		v_6 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y1, z1).x;
		v_7 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y1, z1).x;
		break;
	case Y_VEL:
		v_0 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y0, z0).y;
		v_1 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y0, z0).y;
		v_2 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y0, z1).y;
		v_3 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y0, z1).y;

		v_4 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y1, z0).y;
		v_5 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y1, z0).y;
		v_6 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y1, z1).y;
		v_7 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y1, z1).y;
		break;
	case Z_VEL:
		v_0 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y0, z0).z;
		v_1 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y0, z0).z;
		v_2 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y0, z1).z;
		v_3 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y0, z1).z;
																			
		v_4 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y1, z0).z;
		v_5 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y1, z0).z;
		v_6 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y1, z1).z;
		v_7 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y1, z1).z;
		break;
	}
	float v_0_4 = v_0 * sy + v_4 * ty;
	float v_1_5 = v_1 * sy + v_5 * ty;
	float v_2_6 = v_2 * sy + v_6 * ty;
	float v_3_7 = v_3 * sy + v_7 * ty;

	float v_0_4_1_5 = v_0_4 * sx + v_1_5 * tx;
	float v_2_6_3_7 = v_2_6 * sx + v_3_7 * tx;

	return v_0_4_1_5 * sz + v_2_6_3_7 * tz;
}

SequentialGridBased::SequentialGridBased()
	: ws_lower_bound_(0.0, 0.0, 0.0),
	ws_upper_bound_(10.0, 10.0, 10.0),
	ws_grid_interval_(0.1),
	grid_dim_(100),
	number_of_iterations_(40),
	velocities_((grid_dim_ + 1)* (grid_dim_ + 1)* (grid_dim_ + 1)),
	is_fluid_((grid_dim_) * (grid_dim_) * (grid_dim_)),
	pressures_((grid_dim_)* (grid_dim_)* (grid_dim_)),
	dye_density_((grid_dim_) * (grid_dim_) * (grid_dim_)),
	cell_types_((grid_dim_) * (grid_dim_) * (grid_dim_))
{
	for (int x = 0; x < grid_dim_; x++) {
		for (int y = 0; y < grid_dim_; y++) {
			for (int z = 0; z < grid_dim_; z++) {
				if (x == 0 || y == 0 || z == 0 || x + 1 == grid_dim_ || y + 1 == grid_dim_ || z + 1 == grid_dim_) {
					is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = 0.0f;
					cell_types_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = SOLID;
					SetVelocityIn3DGridCell(velocities_, grid_dim_, x, y, z, glm::vec3(0, 0, 0));
				}
				else {
					is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = 1.0f;
					cell_types_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = FLUID;
					SetVelocityIn3DGridCell(velocities_, grid_dim_, x, y, z, glm::vec3(-0.2f, 0.4f, 0.1f));
					if (y == 1) {
						dye_density_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = 1.0f;
					} else {
						dye_density_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = 0.0f;
					}
				}
				pressures_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = 0.0f;
			}
		}
	}
}

SequentialGridBased::~SequentialGridBased()
{
}

void SequentialGridBased::SetInitialVelocities(const std::vector<glm::vec3>& initial, glm::vec3 lower_bound, glm::vec3 upper_bound, float interval)
{
	grid_dim_ = (upper_bound.x - lower_bound.x) / interval;
	if (initial.size() == (grid_dim_ + 1) * (grid_dim_ + 1) * (grid_dim_ + 1)) {
		velocities_ = initial;
		ws_lower_bound_ = lower_bound;
		ws_upper_bound_ = upper_bound;
		ws_grid_interval_ = interval;
		is_fluid_.clear();
		is_fluid_.resize(grid_dim_ * grid_dim_ * grid_dim_, 0.0f);
		pressures_.clear();
		pressures_.resize(grid_dim_ * grid_dim_ * grid_dim_, 0.0f);
		dye_density_.clear();
		dye_density_.resize(grid_dim_ * grid_dim_ * grid_dim_, 0.0f);
		for (int x = 1; x < grid_dim_ - 1; x++) {
			for (int y = 1; y < grid_dim_ - 1; y++) {
				for (int z = 1; z < grid_dim_ - 1; z++) {
					is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = 1.0f;
				}
			}
		}
	}
}

void SequentialGridBased::TimeStep(float delta)
{
	Integrate(delta, glm::vec3(0.0, -9.8, 0.0));
	SolveIncompressability(delta);
	BorderConditionUpdate();
	AdvectVelocity(delta);
}

std::vector<glm::vec3>* SequentialGridBased::GetGridVelocities()
{
	return &velocities_;
}

unsigned int SequentialGridBased::GetGridDimensions()
{
	return grid_dim_;
}

glm::vec3 SequentialGridBased::GetGridUpperBounds()
{
	return ws_upper_bound_;
}

glm::vec3 SequentialGridBased::GetGridLowerBounds()
{
	return ws_lower_bound_;
}

float SequentialGridBased::GetGridInterval()
{
	return ws_grid_interval_;
}

std::vector<glm::vec3>* SequentialGridBased::GetParticleVelocities()
{
	return nullptr;
}

std::vector<glm::vec3>* SequentialGridBased::GetParticlePositions()
{
	return nullptr;
}

std::vector<float>* SequentialGridBased::GetGridPressures()
{
	return &pressures_;
}

std::vector<float>* SequentialGridBased::GetGridDyeDensities()
{
	return &dye_density_;
}

std::vector<float>* SequentialGridBased::GetGridFluidCells()
{
	return &is_fluid_;
}

void SequentialParticleBased::IntegrateParticles(float delta, glm::vec3 accel)
{
	glm::vec3 adjusted_lower = ws_lower_bound_ + ws_grid_interval_;
	glm::vec3 adjusted_upper = ws_upper_bound_ - ws_grid_interval_;

	// Semi-implicit Euler integration
	for (glm::vec3& vel : particle_vel_) {
		vel += accel * delta;
	}
	for (int i = 0; i < particle_pos_.size(); i++) {
		glm::vec3& pos = particle_pos_[i];
		glm::vec3& vel = particle_vel_[i];
		pos += particle_vel_[i] * delta;
		// Boundary Conditions
		if (pos.x > adjusted_upper.x) {
			pos.x = adjusted_upper.x;
			vel.x = 0.0f;
		}
		if (pos.y > adjusted_upper.y) {
			pos.y = adjusted_upper.y;
			vel.y = 0.0f;
		}
		if (pos.z > adjusted_upper.z) {
			pos.z = adjusted_upper.z;
			vel.z = 0.0f;
		}
		if (pos.x < adjusted_lower.x) {
			pos.x = adjusted_lower.x;
			vel.x = 0.0f;
		}
		if (pos.y < adjusted_lower.y) {
			pos.y = adjusted_lower.y;
			vel.y = 0.0f;
		}
		if (pos.z < adjusted_lower.z) {
			pos.z = adjusted_lower.z;
			vel.z = 0.0f;
		}
	}
}

void SequentialParticleBased::PushApartParticles()
{
	// TODO
}

void SequentialParticleBased::TransferVelocitiesToGrid()
{
	// Transfer particle velocities to grid
	// Update is_fluid_ array

	std::vector<glm::vec3> old_vel = velocities_;
	delta_velocities_.clear();
	delta_velocities_.resize(old_vel.size(), glm::vec3(0, 0, 0));
	velocities_.clear();
	velocities_.resize(old_vel.size(), glm::vec3(0, 0, 0));

	for (int i = 0; i < is_fluid_.size(); i++) {
		cell_types_[i] = is_fluid_[i] == 0.0f ? SOLID : AIR;
	}

	for (int i = 0; i < particle_pos_.size(); i++) {
		glm::vec3 ws_pos = particle_pos_[i];
		glm::vec3 ws_vel = particle_vel_[i];
		ws_pos -= ws_lower_bound_;

		// TODO: handle particles exactly in the upper bound (these particles evalute to grid_dim, which is out of bounds)
		glm::ivec3 grid_pos = glm::clamp(
			glm::ivec3(ws_pos.x / ws_grid_interval_, ws_pos.y / ws_grid_interval_, ws_pos.z / ws_grid_interval_),
			glm::ivec3(0),
			glm::ivec3(grid_dim_ - 1)
		);

		if (cell_types_[grid_pos.x * grid_dim_ * grid_dim_ + grid_pos.y * grid_dim_ + grid_pos.z] == AIR) {
			cell_types_[grid_pos.x * grid_dim_ * grid_dim_ + grid_pos.y * grid_dim_ + grid_pos.z] = FLUID;
		}
	}

	enum CompType {
		X,Y,Z
	};
	static const std::vector<CompType> vel_comps = { X, Y, Z };

	float one_over_ws_interval = 1.0 / ws_grid_interval_;

	for (const CompType& type : vel_comps) {
		glm::vec3 delta = glm::vec3(0, 0, 0);

		switch (type) {
		case X:
			delta.y = ws_grid_interval_ / 2.0f;
			delta.z = ws_grid_interval_ / 2.0f;
			break;
		case Y:
			delta.x = ws_grid_interval_ / 2.0f;
			delta.z = ws_grid_interval_ / 2.0f;
			break;
		case Z:
			delta.y = ws_grid_interval_ / 2.0f;
			delta.x = ws_grid_interval_ / 2.0f;
			break;
		}

		for (int i = 0; i < particle_pos_.size(); i++) {
			glm::vec3 ws_pos = glm::vec3(
				fmax(fmin(particle_pos_[i].x - ws_lower_bound_.x, ws_upper_bound_.x - ws_lower_bound_.x), ws_grid_interval_),
				fmax(fmin(particle_pos_[i].y - ws_lower_bound_.y, ws_upper_bound_.y - ws_lower_bound_.y), ws_grid_interval_),
				fmax(fmin(particle_pos_[i].z - ws_lower_bound_.z, ws_upper_bound_.z - ws_lower_bound_.z), ws_grid_interval_)
			);
			glm::vec3 ws_vel = particle_vel_[i];

			

			unsigned int x0 = fmin((unsigned int)floor(ws_pos.x - delta.x * one_over_ws_interval), grid_dim_ - 1);
			float tx = ((ws_pos.x - delta.x) - ((float)x0) * ws_grid_interval_) * one_over_ws_interval;
			unsigned int x1 = fmin(x0 + 1, grid_dim_ - 1);

			unsigned int y0 = fmin((unsigned int)floor(ws_pos.y - delta.y * one_over_ws_interval), grid_dim_ - 1);
			float ty = ((ws_pos.y - delta.y) - ((float)y0) * ws_grid_interval_) * one_over_ws_interval;
			unsigned int y1 = fmin(y0 + 1, grid_dim_ - 1);

			unsigned int z0 = fmin((unsigned int)floor(ws_pos.z - delta.z * one_over_ws_interval), grid_dim_ - 1);
			float tz = ((ws_pos.z - delta.z) - ((float)z0) * ws_grid_interval_) * one_over_ws_interval;
			unsigned int z1 = fmin(z0 + 1, grid_dim_ - 1);

			float sx = 1.0f - tx;
			float sy = 1.0f - ty;
			float sz = 1.0f - tz;

			float d0 = sx * sy * sz;
			float d1 = tx * sy * sz;
			float d2 = sx * sy * tz;
			float d3 = tx * sy * tz;

			float d4 = sx * ty * sz;
			float d5 = tx * ty * sz;
			float d6 = sx * ty * tz;
			float d7 = tx * ty * tz;

			glm::vec3 comp_vel = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y0, z0);
			glm::vec3 density = glm::vec3(0, 0, 0);
			switch (type)
			{
			case X:
				ws_vel.y = 0.0f;
				ws_vel.z = 0.0f;
				density.x = 1.0f;
				break;
			case Y:
				ws_vel.x = 0.0f;
				ws_vel.z = 0.0f;
				density.y = 1.0f;
				break;
			case Z:
				ws_vel.y = 0.0f;
				ws_vel.x = 0.0f;
				density.z = 1.0f;
				break;
			}
			SetVelocityIn3DGridCell(velocities_, grid_dim_, x0, y0, z0, comp_vel + ws_vel * d0);
			SetVelocityIn3DGridCell(velocities_, grid_dim_, x1, y0, z0, comp_vel + ws_vel * d1);
			SetVelocityIn3DGridCell(velocities_, grid_dim_, x0, y0, z1, comp_vel + ws_vel * d2);
			SetVelocityIn3DGridCell(velocities_, grid_dim_, x1, y0, z1, comp_vel + ws_vel * d3);

			SetVelocityIn3DGridCell(velocities_, grid_dim_, x0, y1, z0, comp_vel + ws_vel * d4);
			SetVelocityIn3DGridCell(velocities_, grid_dim_, x1, y1, z0, comp_vel + ws_vel * d5);
			SetVelocityIn3DGridCell(velocities_, grid_dim_, x0, y1, z1, comp_vel + ws_vel * d6);
			SetVelocityIn3DGridCell(velocities_, grid_dim_, x1, y1, z1, comp_vel + ws_vel * d7);

			SetVelocityIn3DGridCell(particle_densities_, grid_dim_, x0, y0, z0, density * d0);
			SetVelocityIn3DGridCell(particle_densities_, grid_dim_, x1, y0, z0, density * d1);
			SetVelocityIn3DGridCell(particle_densities_, grid_dim_, x0, y0, z1, density * d2);
			SetVelocityIn3DGridCell(particle_densities_, grid_dim_, x1, y0, z1, density * d3);

			SetVelocityIn3DGridCell(particle_densities_, grid_dim_, x0, y1, z0, density * d4);
			SetVelocityIn3DGridCell(particle_densities_, grid_dim_, x1, y1, z0, density * d5);
			SetVelocityIn3DGridCell(particle_densities_, grid_dim_, x0, y1, z1, density * d6);
			SetVelocityIn3DGridCell(particle_densities_, grid_dim_, x1, y1, z1, density * d7);
			// glm::vec3 delta = glm::vec3(fmod(ws_pos.x, ws_grid_interval_), fmod(ws_pos.y, ws_grid_interval_), fmod(ws_pos.z, ws_grid_interval_));
			// glm::vec3 delta = glm::vec3(ws_pos.x - grid_pos.x * ws_grid_interval_, ws_pos.y - grid_pos.y * ws_grid_interval_, ws_pos.z - grid_pos.z * ws_grid_interval_);
		}

		switch (type) {
		case X:
			for (int i = 0; i < velocities_.size(); i++) {
				if (particle_densities_[i].x != 0.0f) {
					velocities_[i].x /= particle_densities_[i].x;
				}
			}		
			break;
		case Y:
			for (int i = 0; i < velocities_.size(); i++) {
				if (particle_densities_[i].y != 0.0f) {
					velocities_[i].y /= particle_densities_[i].y;
				}
			}
			break;
		case Z:
			for (int i = 0; i < velocities_.size(); i++) {
				if (particle_densities_[i].z != 0.0f) {
					velocities_[i].z /= particle_densities_[i].z;
				}
			}
			break;
		}
	}

	for (int x = 0; x < grid_dim_; x++) {

		for (int y = 0; y < grid_dim_; y++) {

			for (int z = 0; z < grid_dim_; z++) {
				bool solid = cell_types_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] == SOLID;
				// Perserve solid cell values
				if (solid || (x > 0 && cell_types_[(x - 1) * grid_dim_ * grid_dim_ + y * grid_dim_ + z] == SOLID)) {
					velocities_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z].x = old_vel[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z].x;
				}
				if (solid || (y > 0 && cell_types_[x * grid_dim_ * grid_dim_ + (y - 1) * grid_dim_ + z] == SOLID)) {
					velocities_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z].y = old_vel[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z].y;
				}
				if (solid || (z > 0 && cell_types_[x * grid_dim_ * grid_dim_ + (y) * grid_dim_ + z - 1] == SOLID)) {
					velocities_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z].z = old_vel[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z].z;
				}
				// Update delta velocities
				delta_velocities_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = velocities_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] - old_vel[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z];
			}
		}
	}
}

void SequentialParticleBased::TransferVelocitiesToParticles(float flip_ratio)
{
	enum CompType {
		X, Y, Z
	};
	static const std::vector<CompType> vel_comps = { X, Y, Z };

	float one_over_ws_interval = 1.0 / ws_grid_interval_;

	for (const CompType& type : vel_comps) {
		glm::vec3 delta = glm::vec3(0, 0, 0);

		switch (type) {
		case X:
			delta.y = ws_grid_interval_ / 2.0f;
			delta.z = ws_grid_interval_ / 2.0f;
			break;
		case Y:
			delta.x = ws_grid_interval_ / 2.0f;
			delta.z = ws_grid_interval_ / 2.0f;
			break;
		case Z:
			delta.y = ws_grid_interval_ / 2.0f;
			delta.x = ws_grid_interval_ / 2.0f;
			break;
		}

		for (int i = 0; i < particle_pos_.size(); i++) {
			glm::vec3 ws_pos = glm::vec3(
				fmax(fmin(particle_pos_[i].x - ws_lower_bound_.x, ws_upper_bound_.x - ws_lower_bound_.x), ws_grid_interval_),
				fmax(fmin(particle_pos_[i].y - ws_lower_bound_.y, ws_upper_bound_.y - ws_lower_bound_.y), ws_grid_interval_),
				fmax(fmin(particle_pos_[i].z - ws_lower_bound_.z, ws_upper_bound_.z - ws_lower_bound_.z), ws_grid_interval_)
			);

			int x0 = fmin((unsigned int)floor(ws_pos.x - delta.x * one_over_ws_interval), grid_dim_ - 1);
			float tx = ((ws_pos.x - delta.x) - ((float)x0) * ws_grid_interval_) * one_over_ws_interval;
			int x1 = fmin(x0 + 1, grid_dim_ - 1);

			int y0 = fmin((unsigned int)floor(ws_pos.y - delta.y * one_over_ws_interval), grid_dim_ - 1);
			float ty = ((ws_pos.y - delta.y) - ((float)y0) * ws_grid_interval_) * one_over_ws_interval;
			int y1 = fmin(y0 + 1, grid_dim_ - 1);

			int z0 = fmin((unsigned int)floor(ws_pos.z - delta.z * one_over_ws_interval), grid_dim_ - 1);
			float tz = ((ws_pos.z - delta.z) - ((float)z0) * ws_grid_interval_) * one_over_ws_interval;
			int z1 = fmin(z0 + 1, grid_dim_ - 1);

			float sx = 1.0f - tx;
			float sy = 1.0f - ty;
			float sz = 1.0f - tz;

			float d0 = sx * sy * sz;
			float d1 = tx * sy * sz;
			float d2 = sx * sy * tz;
			float d3 = tx * sy * tz;

			float d4 = sx * ty * sz;
			float d5 = tx * ty * sz;
			float d6 = sx * ty * tz;
			float d7 = tx * ty * tz;

			glm::ivec3 offset = glm::ivec3(0, 0, 0);
			switch (type) {
			case X:
				offset.x = 1;
				break;
			case Y:
				offset.y = 1;
				break;
			case Z:
				offset.z = 1;
				break;
			}

			float valid0 = cell_types_[(x0) * grid_dim_ * grid_dim_ + (y0) * grid_dim_ + (z0)] != AIR ||
				cell_types_[(x0 + offset.x) * grid_dim_ * grid_dim_ + (y0 + offset.y) * grid_dim_ + (z0 + offset.z)] != AIR ? 1.0f : 0.0f;

			float valid1 = cell_types_[(x1)*grid_dim_ * grid_dim_ + (y0)*grid_dim_ + (z0)] != AIR ||
				cell_types_[(x1 + offset.x) * grid_dim_ * grid_dim_ + (y0 + offset.y) * grid_dim_ + (z0 + offset.z)] != AIR ? 1.0f : 0.0f;

			float valid2 = cell_types_[(x0)*grid_dim_ * grid_dim_ + (y0)*grid_dim_ + (z1)] != AIR ||
				cell_types_[(x0 + offset.x) * grid_dim_ * grid_dim_ + (y0 + offset.y) * grid_dim_ + (z1 + offset.z)] != AIR ? 1.0f : 0.0f;

			float valid3 = cell_types_[(x1)*grid_dim_ * grid_dim_ + (y0)*grid_dim_ + (z1)] != AIR ||
				cell_types_[(x1 + offset.x) * grid_dim_ * grid_dim_ + (y0 + offset.y) * grid_dim_ + (z1 + offset.z)] != AIR ? 1.0f : 0.0f;



			float valid4 = cell_types_[(x0)*grid_dim_ * grid_dim_ + (y1)*grid_dim_ + (z0)] != AIR ||
				cell_types_[(x0 + offset.x) * grid_dim_ * grid_dim_ + (y1 + offset.y) * grid_dim_ + (z0 + offset.z)] != AIR ? 1.0f : 0.0f;
			float valid5 = cell_types_[(x1)*grid_dim_ * grid_dim_ + (y1)*grid_dim_ + (z0)] != AIR ||
				cell_types_[(x1 + offset.x) * grid_dim_ * grid_dim_ + (y1 + offset.y) * grid_dim_ + (z0 + offset.z)] != AIR ? 1.0f : 0.0f;
			float valid6 = cell_types_[(x0)*grid_dim_ * grid_dim_ + (y1)*grid_dim_ + (z1)] != AIR ||
				cell_types_[(x0 + offset.x) * grid_dim_ * grid_dim_ + (y1 + offset.y) * grid_dim_ + (z1 + offset.z)] != AIR ? 1.0f : 0.0f;
			float valid7 = cell_types_[(x1)*grid_dim_ * grid_dim_ + (y1)*grid_dim_ + (z0)] != AIR ||
				cell_types_[(x1 + offset.x) * grid_dim_ * grid_dim_ + (y1 + offset.y) * grid_dim_ + (z1 + offset.z)] != AIR ? 1.0f : 0.0f;

			float d = valid0 * d0 + valid1 * d1 + valid2 * d2 + valid3 * d3 + valid4 * d4 + valid5 * d5 + valid6 * d6 + valid7 * d7;
			glm::vec3 ws_vel = particle_vel_[i];

			float v0 = 0.0f;
			float v1 = 0.0f;
			float v2 = 0.0f;
			float v3 = 0.0f;
			float v4 = 0.0f;
			float v5 = 0.0f;
			float v6 = 0.0f;
			float v7 = 0.0f;

			float dv0 = 0.0f;
			float dv1 = 0.0f;
			float dv2 = 0.0f;
			float dv3 = 0.0f;
			float dv4 = 0.0f;
			float dv5 = 0.0f;
			float dv6 = 0.0f;
			float dv7 = 0.0f;
			switch (type)
			{
			case X:
				ws_vel.y = 0.0f;
				ws_vel.z = 0.0f;
				v0 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y0, z0).x;
				v1 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y0, z0).x;
				v2 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y0, z1).x;
				v3 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y0, z1).x;
				v4 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y1, z0).x;
				v5 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y1, z0).x;
				v6 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y1, z1).x;
				v7 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y1, z1).x;

				dv0 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x0, y0, z0).x;
				dv1 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x1, y0, z0).x;
				dv2 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x0, y0, z1).x;
				dv3 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x1, y0, z1).x;
				dv4 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x0, y1, z0).x;
				dv5 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x1, y1, z0).x;
				dv6 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x0, y1, z1).x;
				dv7 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x1, y1, z1).x;
				break;
			case Y:
				ws_vel.x = 0.0f;
				ws_vel.z = 0.0f;
				v0 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y0, z0).y;
				v1 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y0, z0).y;
				v2 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y0, z1).y;
				v3 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y0, z1).y;
				v4 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y1, z0).y;
				v5 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y1, z0).y;
				v6 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y1, z1).y;
				v7 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y1, z1).y;

				dv0 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x0, y0, z0).y;
				dv1 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x1, y0, z0).y;
				dv2 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x0, y0, z1).y;
				dv3 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x1, y0, z1).y;
				dv4 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x0, y1, z0).y;
				dv5 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x1, y1, z0).y;
				dv6 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x0, y1, z1).y;
				dv7 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x1, y1, z1).y;
				break;
			case Z:
				ws_vel.y = 0.0f;
				ws_vel.x = 0.0f;
				v0 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y0, z0).z;
				v1 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y0, z0).z;
				v2 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y0, z1).z;
				v3 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y0, z1).z;
				v4 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y1, z0).z;
				v5 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y1, z0).z;
				v6 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x0, y1, z1).z;
				v7 = GetVelocityFrom3DGridCell(velocities_, grid_dim_, x1, y1, z1).z;

				dv0 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x0, y0, z0).z;
				dv1 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x1, y0, z0).z;
				dv2 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x0, y0, z1).z;
				dv3 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x1, y0, z1).z;
				dv4 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x0, y1, z0).z;
				dv5 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x1, y1, z0).z;
				dv6 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x0, y1, z1).z;
				dv7 = GetVelocityFrom3DGridCell(delta_velocities_, grid_dim_, x1, y1, z1).z;
				break;
			}
			if (d > 0.0f) {
				float pic_v = (valid0 * v0 * d0
					+ valid1 * v1 * d1 + valid2 * v2 * d2
					+ valid3 * v3 * d3 + valid4 * v4 * d4
					+ valid5 * v5 * d5 + valid6 * v6 * d6
					+ valid7 * v7 * d7) / d;
				float diff = (valid0 * dv0 * d0
					+ valid1 * dv1 * d1 + valid2 * dv2 * d2
					+ valid3 * dv3 * d3 + valid4 * dv4 * d4
					+ valid5 * dv5 * d5 + valid6 * dv6 * d6
					+ valid7 * dv7 * d7) / d;
				float flip_v;

				switch (type) {
				case X:
					flip_v = ws_vel.x + diff;
					particle_vel_[i].x = flip_ratio * flip_v + (1.0f - flip_ratio) * pic_v;
					break;
				case Y:
					flip_v = ws_vel.y + diff;
					particle_vel_[i].y = flip_ratio * flip_v + (1.0f - flip_ratio) * pic_v;
					break;
				case Z:
					flip_v = ws_vel.z + diff;
					particle_vel_[i].z = flip_ratio * flip_v + (1.0f - flip_ratio) * pic_v;
					break;
				}
			}
		}
	}
}

SequentialParticleBased::SequentialParticleBased()
	: seperate_particles_(false)
{
	// TODO
}

SequentialParticleBased::~SequentialParticleBased()
{
}

void SequentialParticleBased::SetInitialVelocities(const std::vector<glm::vec3>& initial, glm::vec3 lower_bound, glm::vec3 upper_bound, float interval)
{
	grid_dim_ = (upper_bound.x - lower_bound.x) / interval;
	ws_lower_bound_ = lower_bound;
	ws_upper_bound_ = upper_bound;
	ws_grid_interval_ = interval;

	velocities_.clear();
	velocities_.resize((grid_dim_ + 1) * (grid_dim_ + 1) * (grid_dim_ + 1), glm::vec3(0, 0, 0));
	is_fluid_.clear();
	is_fluid_.resize(grid_dim_ * grid_dim_ * grid_dim_, 0.0f);
	pressures_.clear();
	pressures_.resize(grid_dim_ * grid_dim_ * grid_dim_, 0.0f);

	for (int x = 0; x < grid_dim_; x++) {
		for (int y = 0; y < grid_dim_; y++) {
			for (int z = 0; z < grid_dim_; z++) {
				if (x == 0 || y == 0 || z == 0 || x + 1 == grid_dim_ || y + 1 == grid_dim_ || z + 1 == grid_dim_) {
					is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = 0.0f;
					cell_types_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = SOLID;
				}
				else {
					is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = 1.0f;
					cell_types_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = AIR;
				}
			}
		}
	}

	particle_densities_.clear();
	particle_densities_.resize((grid_dim_ + 1) * (grid_dim_ + 1) * (grid_dim_ + 1), glm::vec3(0, 0, 0));

	delta_velocities_.clear();
	delta_velocities_.resize((grid_dim_ + 1) * (grid_dim_ + 1) * (grid_dim_ + 1), glm::vec3(0, 0, 0));

	particle_vel_ = initial;
	//particle_pos_.clear();
	//// TODO
	//float particles_per_side = floor(cbrt(initial.size()));
	//glm::vec3 pos = lower_bound + glm::vec3(interval);
	//float side_count_x = 0.0f;
	//float side_count_y = 0.0f;
	//float side_count_z = 0.0f;
	//for (int i = 0; i < initial.size(); i++) {
	//	pos += glm::vec3(
	//		(side_count_x / particles_per_side) * (upper_bound.x - lower_bound.x), 
	//		(side_count_y / particles_per_side) * (upper_bound.y - lower_bound.y),
	//		(side_count_z / particles_per_side) * (upper_bound.z - lower_bound.z)
	//	);
	//	side_count_x++;
	//	if (pos.x > upper_bound.x + interval) {
	//		pos.x = lower_bound.x + interval;
	//		side_count_x = 0.0f;
	//		side_count_y++;
	//	}
	//	if (pos.y > upper_bound.y + interval) {
	//		pos.y = upper_bound.y + interval;
	//		side_count_y = 0.0f;
	//		side_count_z++;
	//	}
	//	if (pos.z > upper_bound.z + interval) {
	//		pos.z = upper_bound.z + interval;
	//		side_count_z = 0.0f;
	//	}
	//	particle_pos_.push_back(pos);
	//	printf("Pushing pos: ");
	//	printf("[ %3.3f %3.3f %3.3f ]\n", pos.x, pos.y, pos.z);
	//}

	// Setting particle positions
	// TODO: under the assumption that all sides are equal
	particle_pos_ = std::vector<glm::vec3>(initial.size());
	float particles_per_side = floor(cbrt(initial.size()));
	int particles_per_side_squared = particles_per_side * particles_per_side;

	int index = 0;
	glm::vec3 grid_interval_magnitude(std::abs(ws_grid_interval_));
	glm::vec3 adjusted_lower = lower_bound + grid_interval_magnitude;
	glm::vec3 adjusted_upper = upper_bound - grid_interval_magnitude;
	float delta = (std::fabs(adjusted_lower.x) + std::fabs(adjusted_upper.x)) / particles_per_side;

	//printf("particles per side = %f\n", particles_per_side);
	//printf("delta = %f\n", delta);

	for (float z = adjusted_lower.z; z < adjusted_upper.z && index < particle_pos_.size(); z += delta) {
		for (float y = adjusted_lower.y; y < adjusted_upper.y && index < particle_pos_.size(); y += delta) {
			for (float x = adjusted_lower.x; x < adjusted_upper.x && index < particle_pos_.size(); x += delta) {
				particle_pos_[index] = glm::vec3(x, y, z);
				++index;
				//printf("Pushing pos at index (%d): ", index);
				//printf("[ %3.3f %3.3f %3.3f ]\n", x, y, z);
			}
			//printf("\n");
		}
	}

	//printf("Set initial particle positions of size: %d\n", particle_pos_.size());
	//printf("Set initial particle velocities of size: %d\n", particle_vel_.size());
}

void SequentialParticleBased::TimeStep(float delta)
{
	IntegrateParticles(delta, glm::vec3(0.0, -9.8, 0.0));
	if (seperate_particles_) {
		PushApartParticles();
	}
	TransferVelocitiesToGrid();
	SolveIncompressability(delta);
	TransferVelocitiesToParticles(0.1);
	BorderConditionUpdate();
	AdvectVelocity(delta);
}

std::vector<glm::vec3>* SequentialParticleBased::GetParticleVelocities()
{
	return &particle_vel_;
}

std::vector<glm::vec3>* SequentialParticleBased::GetParticlePositions()
{
	return &particle_pos_;
}
