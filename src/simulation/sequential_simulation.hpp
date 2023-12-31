#ifndef SEQUENTIAL_IMPLEMENTATION_H
#define SEQUENTIAL_IMPLEMENTATION_H

#include <glm/glm.hpp>
#include <vector>

class Simulation {
public:
	virtual void SetInitialVelocities(const std::vector<glm::vec3>& initial, glm::vec3 lower_bound, glm::vec3 upper_bound, float interval) = 0;
	virtual void TimeStep(float delta) = 0;
	virtual std::vector<glm::vec3>* GetGridVelocities() = 0;
	virtual unsigned int GetGridDimensions() = 0;
	virtual glm::vec3 GetGridUpperBounds() = 0;
	virtual glm::vec3 GetGridLowerBounds() = 0;
	virtual float GetGridInterval() = 0;
	virtual std::vector<float>* GetGridPressures() = 0;
	virtual std::vector<float>* GetGridDyeDensities() = 0;
	virtual std::vector<float>* GetGridFluidCells() = 0;
	virtual std::vector<glm::vec3>* GetParticleVelocities() = 0;
	virtual std::vector<glm::vec3>* GetParticlePositions() = 0;
};

class SequentialGridBased : public Simulation {
private:
protected:
	enum CellType {
		SOLID,
		FLUID,
		AIR
	};

	glm::vec3 ws_lower_bound_;
	glm::vec3 ws_upper_bound_;
	float ws_grid_interval_;

	unsigned int grid_dim_;
	std::vector<glm::vec3> velocities_;
	std::vector<CellType> cell_types_;
	std::vector<float> is_fluid_;
	std::vector<float> pressures_;
	std::vector<float> dye_density_;
	const float density_ = 1000.0f;

	unsigned int number_of_iterations_;

	enum SampleType {
		X_VEL,
		Y_VEL,
		Z_VEL
	};

	void Integrate(float delta, const glm::vec3& acceleration);
	void SolveIncompressability(float delta);
	void BorderConditionUpdate();
	void AdvectVelocity(float delta);

	float GetAvgXVel(unsigned int x, unsigned int y, unsigned int z, unsigned int x_other, unsigned int y_other, unsigned int z_other);
	float GetAvgYVel(unsigned int x, unsigned int y, unsigned int z, unsigned int x_other, unsigned int y_other, unsigned int z_other);
	float GetAvgZVel(unsigned int x, unsigned int y, unsigned int z, unsigned int x_other, unsigned int y_other, unsigned int z_other);
	float SampleGridVelocity(glm::vec3 ws_position, SampleType s);
public:
	SequentialGridBased();
	~SequentialGridBased();

	virtual void SetInitialVelocities(const std::vector<glm::vec3>& initial, glm::vec3 lower_bound, glm::vec3 upper_bound, float interval);
	virtual void TimeStep(float delta);
	virtual std::vector<glm::vec3>* GetGridVelocities();
	virtual unsigned int GetGridDimensions();
	virtual glm::vec3 GetGridUpperBounds();
	virtual glm::vec3 GetGridLowerBounds();
	virtual float GetGridInterval();
	virtual std::vector<glm::vec3>* GetParticleVelocities();
	virtual std::vector<glm::vec3>* GetParticlePositions();
	virtual std::vector<float>* GetGridPressures();
	virtual std::vector<float>* GetGridDyeDensities();
	virtual std::vector<float>* GetGridFluidCells();
};

class SequentialParticleBased : public SequentialGridBased {
private:
	std::vector<glm::vec3> particle_pos_;
	std::vector<glm::vec3> particle_vel_;
	std::vector<glm::vec3> particle_densities_;
	std::vector<glm::vec3> delta_velocities_;
	bool seperate_particles_;

	void IntegrateParticles(float delta, glm::vec3 accel);
	void PushApartParticles();
	void TransferVelocitiesToGrid();
	void TransferVelocitiesToParticles(float flip_ratio);

public:
	SequentialParticleBased();
	~SequentialParticleBased();
	virtual void SetInitialVelocities(const std::vector<glm::vec3>& initial, glm::vec3 lower_bound, glm::vec3 upper_bound, float interval);
	virtual void TimeStep(float delta);
	virtual std::vector<glm::vec3>* GetParticleVelocities();
	virtual std::vector<glm::vec3>* GetParticlePositions();
};

/**
 * @brief
 * Retrieve the velocity given the indices for a flat 3D velocity grid.
 *
 * @param grid - Velocity grid
 * @param dim - Dimensions of the grid
 * @param x - Cell x index (row)
 * @param y - Cell y Index (column)
 * @param z - Cell z Index (depth)
 *
 * @return GLM Vector containing the 3 velocities of a grid cell.
 */
glm::vec3 GetVelocityFrom3DGridCell(const std::vector<glm::vec3>& grid,
	const unsigned int dim,
	const unsigned int x,
	const unsigned int y,
	const unsigned int z);

/**
 * @brief
 * Set the velocity at the given indices if a flat 3D velocity grid.
 *
 * @param grid - Mutable velocity grid
 * @param dim - Dimensions of the grid
 * @param x - Cell x index (row)
 * @param y - Cell y Index (column)
 * @param z - Cell z Index (depth)
 * @param new_velocity - New velocity for the given index
 */
void SetVelocityIn3DGridCell(std::vector<glm::vec3>& grid,
	const unsigned int dim,
	const unsigned int x,
	const unsigned int y,
	const unsigned int z,
	glm::vec3 new_velocity);

/**
 * @brief
 * Get the divergence (total outflow) given a cell.
 *
 * @param grid - Velocity grid
 * @param dim - Dimensions of the grid
 * @param x - Cell x index (row)
 * @param y - Cell y Index (column)
 * @param z - Cell z Index (depth)
 *
 * @return Divergence value.
 */
float GetDivergence(const std::vector<glm::vec3>& grid,
	const unsigned int dim,
	const unsigned int x,
	const unsigned int y,
	const unsigned int z);

float GetFloatValFrom3DGridCell(const std::vector<float>& grid,
	const unsigned int dim,
	const unsigned int x,
	const unsigned int y,
	const unsigned int z);

#endif // !SEQUENTIAL_IMPLEMENTATION_H