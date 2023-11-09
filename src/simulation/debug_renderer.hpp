#ifndef DEBUG_RENDERER_H
#define DEBUG_RENDERER_H

#include <unordered_set>

#include "../rendering/shader.hpp"
#include "../rendering/texture.hpp"

class DebugArrow {

};

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
	Shader origin_shader_;

	Shader grid_shader_;
	Shader grid_velocity_shader_;
	Shader grid_dye_shader_;

	Shader particle_shader_;
	Shader particle_velocity_shader_;

	std::unordered_set<DebugView> active_shaders_; // If contained in the set, the shader is active

	glm::mat4 cached_view_;
	glm::mat4 cached_proj_;

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