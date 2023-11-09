#ifndef DEBUG_RENDERER_H
#define DEBUG_RENDERER_H

#include <set>
#include <vector>

#include "../rendering/shader.hpp"
#include "../rendering/texture.hpp"

class DebugRenderer {
public:
	enum DebugView {
		ORIGIN,
		GRID,
		GRID_VELOCITIES,
		GRID_DYE,
		PARTICLES,
		PARTICLE_VELOCITIES,
	};
private:
	struct DebugLineVert {
		glm::vec3 pos_;
		glm::vec3 color_;
		DebugLineVert(const glm::vec3& pos, const glm::vec3& color) : pos_(pos), color_(color) {}
	};
	struct DebugArrowVert {
		glm::vec3 pos_;
		glm::vec3 color_;
		DebugArrowVert(const glm::vec3& pos, const glm::vec3& color) : pos_(pos), color_(color) {}
	};
	glm::vec3 ws_grid_lower_bound_;
	glm::vec3 ws_grid_upper_bound_;
	float ws_grid_cell_size_;


	Shader debug_line_shader_;
	GLuint VAO_grid_lines_;
	GLuint VBO_grid_lines_;
	GLuint EBO_grid_lines_;
	std::vector<DebugLineVert> grid_line_vertices_;
	std::vector<unsigned int> grid_line_indices_;

	GLuint VAO_grid_arrows_;
	GLuint VBO_grid_arrows_;
	GLuint EBO_grid_arrows_;

	Shader debug_particle_shader_;


	std::set<DebugView> active_views_; // If contained in the set, the view is active

	glm::mat4 cached_view_;
	glm::mat4 cached_proj_;

	bool UpdateGridLines();

public:
	DebugRenderer();
	~DebugRenderer();

	bool SetGridBoundaries(const glm::vec3& lower_left, const glm::vec3& upper_right);
	bool SetGridCellInterval(const float cell_size);
	bool SetGridVelocities(const Texture3D& grid_velocities);
	bool SetGridDye(const Texture3D& grid_dye);

	bool SetParticlePositions(const Texture2D& particle_positions);
	bool SetParticleVelocities(const Texture2D& particle_velocities);

	bool SetViewMat(const glm::mat4& view);
	bool SetProjectionMat(const glm::mat4 proj);

	bool ToggleDebugView(DebugView view_toggle);

	bool Draw();
};

#endif