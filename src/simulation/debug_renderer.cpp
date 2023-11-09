#include "debug_renderer.hpp"

DebugRenderer::DebugRenderer() : 
	origin_shader_("debug/origin.vert", "debug/origin.frag"),
	grid_shader_("debug/grid.vert", "debug/grid.frag"),
	grid_velocity_shader_("debug/grid_vel.vert", "debug/grid_vel.frag"),
	grid_dye_shader_("debug/grid_dye.vert", "debug/grid_dye.frag"),
	particle_shader_("debug/particle.vert", "debug/particle.frag"),
	particle_velocity_shader_("debug/particle_vel.vert", "debug/particle_vel.frag")
{

}

DebugRenderer::~DebugRenderer()
{

}

bool DebugRenderer::SetGridBoundaries(const glm::vec3& lower_left, const glm::vec3& upper_right)
{
	return false;
}

bool DebugRenderer::SetGridCellInterval(const float cell_size)
{
	return false;
}

bool DebugRenderer::SetGridVelocities(const Texture3D& grid_velocities)
{
	return false;
}

bool DebugRenderer::SetGridDye(const Texture3D& grid_dye)
{
	return false;
}

bool DebugRenderer::SetParticlePositions(const Texture2D& particle_positions)
{
	return false;
}

bool DebugRenderer::SetParticleVelocities(const Texture2D& particle_velocities)
{
	return false;
}

bool DebugRenderer::SetViewMat(const glm::mat4& view)
{
	cached_view_ = view;
	// TODO: Update uniforms for those shaders who need it
	return true;
}

bool DebugRenderer::SetProjectionMat(const glm::mat4 proj)
{
	cached_proj_ = proj;
	// TODO: Update uniforms for those shaders who need it
	return true;
}

bool DebugRenderer::ToggleDebugView(DebugView view_toggle)
{
	if (active_shaders_.count(view_toggle) != 0) {
		active_shaders_.erase(view_toggle);
	} else {
		active_shaders_.insert(view_toggle);
	}
	return true;
}

bool DebugRenderer::Draw()
{
	for (auto active : active_shaders_) {
		switch (active) {
		case ORIGIN:
			break;
		case GRID:
			break;
		case GRID_VELOCITIES:
			break;
		case GRID_DYE:
			break;
		case PARTICLES:
			break;
		case PARTICLE_VELOCITIES:
			break;
		}
	}
	return true;
}
