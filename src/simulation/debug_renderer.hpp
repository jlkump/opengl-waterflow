#ifndef DEBUG_RENDERER_H
#define DEBUG_RENDERER_H

#include <set>
#include <vector>

#include "../rendering/shader.hpp"
#include "../rendering/texture.hpp"
#include "../rendering/display_text.hpp"

#define MAX_DEBUG_GRID_ARROWS 40
#define MAX_DEBUG_GRID_LINES 256

class DebugRenderer {
public:
	enum DebugView {
		ORIGIN,
		GRID,
		GRID_VELOCITIES,
		GRID_DYE,
		PARTICLES,
		PARTICLE_VELOCITIES,
		FRAME_TIME,
	};
private:
	const float k_line_instance_verts_[108] = {
		0.0f, 0.0f, 0.0f, // triangle 1 : begin
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f, 0.0f, // triangle 2 : begin
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, // triangle 2 : end
		1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f
	};

	glm::vec3 ws_grid_lower_bound_;
	glm::vec3 ws_grid_upper_bound_;
	float ws_grid_cell_size_;

	Shader debug_line_shader_;
	GLuint VAO_origin_lines_;
	GLuint VBO_origin_line_instance_;
	GLuint VBO_origin_line_mats_;
	GLuint VBO_origin_line_colors_;

	GLuint VAO_grid_lines_;
	GLuint VBO_line_instance_;
	GLuint VBO_line_mats_;
	GLuint VBO_line_color_;
	int grid_line_elements_;

	// Instance an arrow for each velocity
	Shader debug_grid_vel_shader_;
	GLuint VAO_grid_arrows_;
	GLuint VBO_grid_arrow_instances_;
	GLuint VBO_grid_arrow_mats_;
	GLuint VBO_grid_arrow_colors_;
	int grid_arrow_elements_;
	int grid_arrow_instance_num_;

	glm::ivec3 vel_texture_dimensions_;

	Shader debug_particle_shader_;


	std::set<DebugView> active_views_; // If contained in the set, the view is active

	glm::mat4 cached_view_;
	glm::mat4 cached_proj_;

	DisplayText frame_time_display_;


	void UpdateGridLines();

	void SetupOriginBuffers();
	void SetupGridLineBuffers();
	void MakeInstanceArrow(std::vector<glm::vec3>& verts);
	void SetupGridVelocityBuffers();

public:
	DebugRenderer();
	~DebugRenderer();
	
	bool SetGridBoundaries(const glm::vec3& lower_left, const glm::vec3& upper_right, const float interval);
	bool SetGridVelocities(const std::vector<glm::vec3>& grid_velocities, const unsigned int grid_dimensions);
	bool SetGridVelocities(Texture3D& grid_velocities);
	bool SetGridDye(const Texture3D& grid_dye);

	bool SetParticlePositions(const Texture2D& particle_positions);
	bool SetParticleVelocities(const Texture2D& particle_velocities);

	bool SetView(const glm::mat4& view);
	bool SetProjection(const glm::mat4& proj);

	void ToggleDebugView(DebugView view_toggle);
	void UpdateFrameTime(float frame_time);

	bool Draw();
};

#endif