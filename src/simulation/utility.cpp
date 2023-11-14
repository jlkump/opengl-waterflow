#include "utility.hpp"

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

int GetDivergence(const std::vector<glm::vec3>& grid,
	const unsigned int dim,
	const unsigned int x,
	const unsigned int y,
	const unsigned int z) {

	// TODO
	return 0;
}
