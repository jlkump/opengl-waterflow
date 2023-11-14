#ifndef UTILITY_H
#define UTILITY_H

#include <glm/glm.hpp>
#include <vector>

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
int GetDivergence(const std::vector<glm::vec3>& grid,
				  const unsigned int dim,
				  const unsigned int x,
				  const unsigned int y,
				  const unsigned int z);

#endif // UTILITY_H
