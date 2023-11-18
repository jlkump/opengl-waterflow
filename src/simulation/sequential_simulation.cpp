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
					float s = is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z];
					if (s == 0.0f) {
						continue;
					}
					// TODO: DEBUG Might want to sum total and check instead
					float s_x_neg = is_fluid_[(x-1) * grid_dim_ * grid_dim_ + y * grid_dim_ + z];
					float s_x_pos = is_fluid_[(x+1) * grid_dim_ * grid_dim_ + y * grid_dim_ + z];
					float s_y_neg = is_fluid_[x * grid_dim_ * grid_dim_ + (y-1) * grid_dim_ + z];
					float s_y_pos = is_fluid_[x * grid_dim_ * grid_dim_ + (y+1) * grid_dim_ + z];
					float s_z_neg = is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + (z-1)];
					float s_z_pos = is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + (z+1)];

					s = s_x_neg + s_x_pos + s_y_neg + s_y_pos + s_z_neg + s_z_pos;

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

	ws_pos.x = fmax(fmin(ws_pos.x, ws_upper_bound_.x), ws_grid_interval_);
	ws_pos.y = fmax(fmin(ws_pos.y, ws_upper_bound_.y), ws_grid_interval_);
	ws_pos.z = fmax(fmin(ws_pos.z, ws_upper_bound_.z), ws_grid_interval_);
	
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
	: ws_lower_bound_(-1.0, -1.0, -1.0),
	ws_upper_bound_(1.0, 1.0, 1.0),
	ws_grid_interval_(0.5),
	grid_dim_(4),
	number_of_iterations_(40),
	velocities_((grid_dim_ + 1)* (grid_dim_ + 1)* (grid_dim_ + 1)),
	is_fluid_((grid_dim_) * (grid_dim_) * (grid_dim_)),
	pressures_((grid_dim_)* (grid_dim_)* (grid_dim_)),
	dye_density_((grid_dim_)* (grid_dim_)* (grid_dim_))
{
	for (int x = 0; x < grid_dim_; x++) {
		for (int y = 0; y < grid_dim_; y++) {
			for (int z = 0; z < grid_dim_; z++) {
				if (x == 0 || y == 0 || z == 0 || x + 1 == grid_dim_ || y + 1 == grid_dim_ || z + 1 == grid_dim_) {
					is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = 0.0f;
					SetVelocityIn3DGridCell(velocities_, grid_dim_, x, y, z, glm::vec3(0, 0, 0));
				}
				else {
					is_fluid_[x * grid_dim_ * grid_dim_ + y * grid_dim_ + z] = 1.0f;
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

void SequentialGridBased::SetInitialVelocities(const std::vector<glm::vec3>& initial, glm::vec3 lower_bound, glm::vec3 upper_bound)
{

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

float SequentialGridBased::GetGrindInterval()
{
	return ws_grid_interval_;
}

std::vector<glm::vec3>* SequentialGridBased::GetParticleVelocities()
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
