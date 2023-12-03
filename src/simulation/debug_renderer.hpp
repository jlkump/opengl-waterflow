#ifndef DEBUG_RENDERER_H
#define DEBUG_RENDERER_H

#include <set>
#include <vector>

#include "../rendering/shader.hpp"
#include "../rendering/texture.hpp"
#include "../rendering/display_text.hpp"
#include "../simulation/water_particle_renderer.hpp"

#define MAX_DEBUG_GRID_ARROWS 4096
#define MAX_DEBUG_AXIS_GRID_ARROWS 4096
#define MAX_DEBUG_GRID_LINES 4096
#define MAX_DEBUG_GRID_CELLS 1024
#define MAX_DEBUG_PARTICLES (512 * 512)

class DebugRenderer {
public:
	enum DebugView {
		ORIGIN,
		GRID,
		GRID_VELOCITIES,
		GRID_AXIS_VELOCITIES,
		GRID_CELL,
		PARTICLES,
		PARTICLE_VELOCITIES,
		FRAME_TIME,
	};
	enum GridCellView {
		NONE,
		DYE,
		PRESSURE,
		IS_FLUID,
	};
private:
	const float k_cube_verts[108] = {
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

	const float k_square_verts[18] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};

	glm::vec3 ws_grid_lower_bound_;
	glm::vec3 ws_grid_upper_bound_;
	float ws_grid_cell_size_;

	std::set<DebugView> active_views_; // If contained in the set, the view is active
	GridCellView active_cell_view_;

	glm::mat4 cached_view_;
	glm::mat4 cached_proj_;

	DisplayText frame_time_display_;

	// This shader is used for all instanced debug models
	// It does a simple color shading and world-placement based on a given model matrix.
	Shader debug_instance_shader_;

	// Origin
	GLuint VAO_origin_lines_;
	GLuint VBO_origin_line_instance_;
	GLuint VBO_origin_line_mats_;
	GLuint VBO_origin_line_colors_;

	// Grid lines 
	GLuint VAO_grid_lines_;
	GLuint VBO_line_instance_;
	GLuint VBO_line_mats_;
	GLuint VBO_line_color_;
	int grid_line_elements_;

	// Grid arrows
	int grid_arrow_instance_num_; // Number of verts in the arrow instance
	GLuint VAO_grid_arrows_;
	GLuint VBO_grid_arrow_instance_;
	GLuint VBO_grid_arrow_mats_;
	GLuint VBO_grid_arrow_colors_;
	int grid_arrow_elements_;

	// Axis aligned grid arrows
	GLuint VAO_grid_axis_arrows_;
	GLuint VBO_grid_axis_arrow_instance_;
	GLuint VBO_grid_axis_arrow_mats_;
	GLuint VBO_grid_axis_arrow_colors_;
	int grid_axis_arrow_elements_;

	// Cell fluid visualization
	// Only the float value and uniform color values need to be updated when the display for type of 
	// visualization changes.
	Shader debug_grid_cell_shader_;
	GLuint VAO_grid_cell_;
	GLuint VBO_grid_cell_instance_;
	GLuint VBO_grid_cell_mats_;
	GLuint VBO_grid_cell_float_val_; 
	int grid_cell_elements_;

	// Cell particle sprite visualization
	Shader debug_particle_shader_;
	GLuint VAO_particle_sprite_;
	GLuint VBO_particle_sprite_instance_;
	GLuint VBO_particle_sprite_pos_;
	GLuint VBO_particle_sprite_color_;
	int particle_sprite_elements_;

	// Particle velocities (arrows)
	GLuint VAO_particle_arrows_;
	GLuint VBO_particle_arrow_instance_;
	GLuint VBO_particle_arrow_mats_;
	GLuint VBO_particle_arrow_colors_;

	void UpdateGridLines();
	void UpdateGridAxisVelocities(const std::vector<glm::vec3>& velocities, const unsigned int grid_dim);
	void UpdateGridVelocities(const std::vector<glm::vec3>& velocities, const unsigned int grid_dim);
	void UpdateGridCells(const unsigned int grid_dim);
	void UpdateGridCellFloats(const std::vector<float>& floats, const unsigned int grid_dim);

	void SetVariableDefaults();
	void SetupOriginBuffers();
	void SetupGridLineBuffers();
	void MakeInstanceArrow(std::vector<glm::vec3>& verts);
	void SetupGridVelocityBuffers();
	void SetupGridAxisVelocityBuffers();
	void SetupGridCellBuffers();
	void SetupParticleSpriteBuffers();
	void SetupParticleVelocityBuffers();

public:
	DebugRenderer();
	~DebugRenderer();
	
	void SetGridBoundaries(const glm::vec3& ws_low_bound, const glm::vec3& ws_upper_bound, const float interval);
	void SetGridVelocities(const std::vector<glm::vec3>& grid_velocities, const unsigned int grid_dimensions);
	void SetGridPressures(const std::vector<float>& grid_pressures, const unsigned int grid_dimensions);
	void SetGridDyeDensities(const std::vector<float>& grid_dyes, const unsigned int grid_dimensions);
	void SetGridFluidCells(const std::vector<float>& grid_fluid, const unsigned int grid_dimensions);

	void SetParticlePositions(const std::vector<glm::vec3>& particle_pos);
	void SetParticleVelocities(const std::vector<glm::vec3>& particle_pos, const std::vector<glm::vec3>& particle_vel);

	bool SetView(const glm::mat4& view);
	bool SetProjection(const glm::mat4& proj);

	void ToggleDebugView(DebugView view_toggle);
	bool IsDebugViewActive(DebugView view);
	void SetDebugCellView(GridCellView view);
	bool IsCellViewActive(GridCellView view);
	GridCellView GetCellViewActive();
	void UpdateFrameTime(float frame_time);

	bool Draw();
};

#endif