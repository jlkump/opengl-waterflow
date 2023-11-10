#include "debug_renderer.hpp"
#include "debug_renderer.hpp"
#include "debug_renderer.hpp"

bool DebugRenderer::MakeLine(std::vector<DebugLineVert>& verts, std::vector<unsigned int>& indices, 
	int& index, glm::vec3 start_pos, glm::vec3 end_pos, glm::vec3 color, float thickness, glm::vec3 view_direction)
{
	glm::vec3 line_thickness_dir = glm::normalize(glm::cross((end_pos - start_pos), view_direction)) * thickness;
	verts.push_back(DebugLineVert(start_pos + line_thickness_dir, color));
	verts.push_back(DebugLineVert(start_pos - line_thickness_dir, color));
	verts.push_back(DebugLineVert(end_pos + line_thickness_dir, color));
	verts.push_back(DebugLineVert(end_pos - line_thickness_dir, color));

	indices.push_back(index);
	indices.push_back(index + 1);
	indices.push_back(index + 2);
	indices.push_back(index + 3);
	indices.push_back(index + 2);
	indices.push_back(index + 1);
	index += 4;
	return true;
}

// Only called once really since we instance multiple of a single arrow's vertices
bool DebugRenderer::MakeArrow(std::vector<glm::vec3>& verts, std::vector<unsigned int>& indices, float thickness)
{
	int index;
	glm::vec3 norm = glm::vec3(0, 0, 1);
	glm::vec3 start_pos = glm::vec3(0, 0, 0);
	glm::vec3 end_pos = glm::vec3(0, 1, 0);
	glm::vec3 thickness_dir = glm::normalize(glm::cross((end_pos - start_pos), norm)) * thickness;
	glm::vec3 arrow_head_start = glm::vec3(0, 0.95, 0.0);
	float arrow_wideness = 0.05f;

	verts.push_back(start_pos - thickness_dir);
	verts.push_back(start_pos + thickness_dir);
	verts.push_back(end_pos + thickness_dir);
	verts.push_back(end_pos - thickness_dir);
	// Inefficenienct slightly since we ignore index buffer
	verts.push_back(end_pos + thickness_dir);
	verts.push_back(start_pos - thickness_dir);


	//indices.push_back(index);
	//indices.push_back(index + 1);
	//indices.push_back(index + 2);
	//indices.push_back(index + 3);
	//indices.push_back(index + 2);
	//indices.push_back(index + 1);
	//index += 4;

	verts.push_back(end_pos + glm::vec3(0, 1, 0) * thickness*2.0f);
	verts.push_back(end_pos);
	verts.push_back(arrow_head_start + glm::normalize(thickness_dir) * arrow_wideness);
	verts.push_back(arrow_head_start + glm::normalize(thickness_dir) * arrow_wideness + thickness_dir*2.0f);
	// inefficent slightly
	verts.push_back(arrow_head_start + glm::normalize(thickness_dir) * arrow_wideness);
	verts.push_back(end_pos + glm::vec3(0, 1, 0) * thickness*2.0f);

	//indices.push_back(index);
	//indices.push_back(index + 1);
	//indices.push_back(index + 2);
	//indices.push_back(index + 3);
	//indices.push_back(index + 2);
	//indices.push_back(index + 1);
	//index += 4;

	verts.push_back(end_pos + glm::vec3(0, 1, 0) * thickness*2.0f);
	verts.push_back(end_pos);
	verts.push_back(arrow_head_start - glm::normalize(thickness_dir) * arrow_wideness);
	verts.push_back(arrow_head_start - glm::normalize(thickness_dir) * arrow_wideness - thickness_dir*2.0f);
	// Inefficent slightly
	verts.push_back(arrow_head_start - glm::normalize(thickness_dir) * arrow_wideness);
	verts.push_back(end_pos + glm::vec3(0, 1, 0) * thickness*2.0f);

	//indices.push_back(index);
	//indices.push_back(index + 1);
	//indices.push_back(index + 2);
	//indices.push_back(index + 3);
	//indices.push_back(index + 2);
	//indices.push_back(index + 1);
	//index += 4;

	return false;
}

bool DebugRenderer::UpdateGridLines()
{
	int grid_size = (ws_grid_upper_bound_.x - ws_grid_lower_bound_.x) / ws_grid_cell_size_ + 1;
	glm::vec3 grid_line_color = glm::vec3(156.0 / 256.0, 158.0 / 256.0, 136.0 / 256.0);
	glm::vec3 cam_view_dir_ = cached_view_[2];
	printf("Camera view direction is assumed as (%f %f %f)\n", cam_view_dir_.x, cam_view_dir_.y, cam_view_dir_.z);
	float line_thickness = 0.01;

	grid_line_vertices_.clear();
	grid_line_indices_.clear();

	int z_idx = 0;
	int index = 0;
	for (float z = ws_grid_lower_bound_.z; z <= ws_grid_upper_bound_.z; z += ws_grid_cell_size_) {
		int y_idx = 0;
		for (float y = ws_grid_lower_bound_.y; y <= ws_grid_upper_bound_.y; y += ws_grid_cell_size_) {
			int x_idx = 0;
			for (float x = ws_grid_lower_bound_.x; x <= ws_grid_upper_bound_.x; x += ws_grid_cell_size_) {
				if (x_idx + 1 < grid_size) {
					MakeLine(grid_line_vertices_, grid_line_indices_, index, 
						glm::vec3(x, y, z), glm::vec3(x + ws_grid_cell_size_, y, z), grid_line_color, line_thickness, cam_view_dir_);
				}
				if (y_idx + 1 < grid_size) {
					MakeLine(grid_line_vertices_, grid_line_indices_, index,
						glm::vec3(x, y, z), glm::vec3(x, y + ws_grid_cell_size_, z), grid_line_color, line_thickness, cam_view_dir_);
				}
				if (z_idx + 1 < grid_size) {
					MakeLine(grid_line_vertices_, grid_line_indices_, index,
						glm::vec3(x, y, z), glm::vec3(x, y, z + ws_grid_cell_size_), grid_line_color, line_thickness, cam_view_dir_);
				}
				x_idx++;
			}
			y_idx++;
		}
		z_idx++;
	}

	glBindVertexArray(VAO_grid_lines_);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_lines_);
	glBufferData(GL_ARRAY_BUFFER, grid_line_vertices_.size() * sizeof(DebugLineVert), &grid_line_vertices_[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_grid_lines_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, grid_line_indices_.size() * sizeof(unsigned int), &grid_line_indices_[0], GL_STATIC_DRAW);

	// vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugLineVert), (void*)0);
	// vertex texture coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DebugLineVert), (void*)offsetof(DebugLineVert, color_));

	glBindVertexArray(0);

	return true;
}

bool DebugRenderer::UpdateArrowPositions()
{
	arrow_positions_.clear();
	arrow_colors_.clear();
	arrow_indexes_.clear();

	float offset = ws_grid_cell_size_ / 2.0;
	float grid_side_size = ws_grid_upper_bound_.x - ws_grid_lower_bound_.x;
	int grid_size = (ws_grid_upper_bound_.x - ws_grid_lower_bound_.x) / ws_grid_cell_size_ + 1;

	int z_idx = 0;
	for (float z = ws_grid_lower_bound_.z; z <= ws_grid_upper_bound_.z; z += ws_grid_cell_size_) {
		int y_idx = 0;
		for (float y = ws_grid_lower_bound_.y; y <= ws_grid_upper_bound_.y; y += ws_grid_cell_size_) {
			int x_idx = 0;
			for (float x = ws_grid_lower_bound_.x; x <= ws_grid_upper_bound_.x; x += ws_grid_cell_size_) {
				if (x_idx + 1 < grid_size) {
					arrow_positions_.push_back(glm::vec3(x + offset, y, z));
					arrow_colors_.push_back(glm::vec3(1, 0, 0));
					arrow_indexes_.push_back(glm::vec3(
						(x - ws_grid_lower_bound_.x) / grid_side_size, 
						(y - ws_grid_lower_bound_.y) / grid_side_size, 
						(z - ws_grid_lower_bound_.z) / grid_side_size));
				}
				if (y_idx + 1 < grid_size) {
					arrow_positions_.push_back(glm::vec3(x, y + offset, z));
					arrow_colors_.push_back(glm::vec3(0, 1, 0));
					arrow_indexes_.push_back(glm::vec3(
						(x - ws_grid_lower_bound_.x) / grid_side_size,
						(y - ws_grid_lower_bound_.y) / grid_side_size, 
						(z - ws_grid_lower_bound_.z) / grid_side_size));
				}
				if (z_idx + 1 < grid_size) {
					arrow_positions_.push_back(glm::vec3(x, y, z + offset));
					arrow_colors_.push_back(glm::vec3(0, 0, 1));
					arrow_indexes_.push_back(glm::vec3(
						(x - ws_grid_lower_bound_.x) / grid_side_size,
						(y - ws_grid_lower_bound_.y) / grid_side_size, 
						(z - ws_grid_lower_bound_.z) / grid_side_size));
				}
				x_idx++;
			}
			y_idx++;
		}
		z_idx++;
	}
	printf("Arrow verts:\n");
	for (auto& v : arrow_positions_) {
		printf("(%4.4f, %4.4f, %4.4f)\n", v.x, v.y, v.z);
	}
	glBindVertexArray(VAO_grid_arrows_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_pos_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, arrow_positions_.size() * sizeof(glm::vec3), (void*)&arrow_positions_[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_colors_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, arrow_colors_.size() * sizeof(glm::vec3), (void*)&arrow_colors_[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_indices_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, arrow_indexes_.size() * sizeof(glm::vec3), (void*)&arrow_indexes_[0]);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

void DebugRenderer::InitializeDebugArrow()
{
	arrow_positions_.clear();
	arrow_colors_.clear();
	arrow_indexes_.clear();

	arrow_positions_.push_back(glm::vec3(-0.5, -1, -1));
	arrow_colors_.push_back(glm::vec3(0.1, 0.4, 0.5));
	arrow_indexes_.push_back(glm::vec3(0, 0, 0));

	glBindVertexArray(VAO_grid_arrows_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_pos_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, arrow_positions_.size() * sizeof(glm::vec3), (void*)&arrow_positions_[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_colors_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, arrow_colors_.size() * sizeof(glm::vec3), (void*)&arrow_colors_[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_indices_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, arrow_indexes_.size() * sizeof(glm::vec3), (void*)&arrow_indexes_[0]);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

DebugRenderer::DebugRenderer() :
	debug_line_shader_("debug/line.vert", "debug/line.frag"),
	debug_grid_vel_shader_("debug/grid_arrows.vert", "debug/grid_arrows.frag"),
	debug_particle_shader_("debug/particle.vert","debug/particle.frag"),
	ws_grid_cell_size_(1.0),
	ws_grid_lower_bound_(glm::vec3(-1, -1, -1)),
	ws_grid_upper_bound_(glm::vec3(1, 1, 1)),
	cached_view_(glm::mat4()),
	cached_proj_(glm::mat4()),
	VAO_grid_lines_(0),
	VBO_grid_lines_(0),
	EBO_grid_lines_(0),
	VBO_grid_arrow_instances_(0),
	VBO_grid_arrow_indices_(0),
	VBO_grid_arrow_pos_(0),
	VBO_grid_arrow_colors_(0)
{
	// Setup for the grid lines
	glGenVertexArrays(1, &VAO_grid_lines_);
	glGenBuffers(1, &VBO_grid_lines_);
	glGenBuffers(1, &EBO_grid_lines_);

	UpdateGridLines();

	// Setup for the arrows
	glGenVertexArrays(1, &VAO_grid_arrows_);
	glGenBuffers(1, &VBO_grid_arrow_instances_);
	glGenBuffers(1, &VBO_grid_arrow_pos_);
	glGenBuffers(1, &VBO_grid_arrow_colors_);
	glGenBuffers(1, &VBO_grid_arrow_indices_);

	MakeArrow(instance_arrow_verts_, instance_arrow_indices_, 0.01);

	glBindVertexArray(VAO_grid_arrows_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_instances_);
	glBufferData(GL_ARRAY_BUFFER, instance_arrow_verts_.size() * sizeof(glm::vec3), &instance_arrow_verts_[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_pos_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_colors_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_indices_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBindVertexArray(0);
}

DebugRenderer::~DebugRenderer()
{
	glDeleteBuffers(1, &VAO_grid_lines_);
	glDeleteBuffers(1, &VBO_grid_lines_);
	glDeleteBuffers(1, &EBO_grid_lines_);

	glDeleteBuffers(1, &VAO_grid_arrows_);
	glDeleteBuffers(1, &VBO_grid_arrow_instances_);
	glDeleteBuffers(1, &VBO_grid_arrow_pos_);
	glDeleteBuffers(1, &VBO_grid_arrow_colors_);
	glDeleteBuffers(1, &VBO_grid_arrow_indices_);
}

bool DebugRenderer::SetGridBoundaries(const glm::vec3& lower_left, const glm::vec3& upper_right)
{
	ws_grid_lower_bound_ = lower_left;
	ws_grid_upper_bound_ = upper_right;
	UpdateGridLines();
	UpdateArrowPositions();
	return true;
}

bool DebugRenderer::SetGridCellInterval(const float cell_size)
{
	ws_grid_cell_size_ = cell_size;
	UpdateGridLines();
	UpdateArrowPositions();
	return true;
}

bool DebugRenderer::SetGridVelocities(Texture3D& grid_velocities)
{
	vel_texture_dimensions_ = grid_velocities.GetDimensions();
	debug_grid_vel_shader_.SetUniformTexture3D("velocities", grid_velocities, GL_TEXTURE0);
	UpdateArrowPositions();
	return true;
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
	// Update the uniforms for shaders
	debug_line_shader_.SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	debug_grid_vel_shader_.SetUniformMatrix4fv("view", cached_view_);
	debug_grid_vel_shader_.SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	return true;
}

bool DebugRenderer::SetProjectionMat(const glm::mat4 proj)
{
	cached_proj_ = proj;
	// Update the uniforms for shaders
	debug_line_shader_.SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	debug_grid_vel_shader_.SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	return true;
}

bool DebugRenderer::ToggleDebugView(DebugView view_toggle)
{
	if (active_views_.count(view_toggle) != 0) {
		active_views_.erase(view_toggle);
	} else {
		active_views_.insert(view_toggle);
	}
	return true;
}

bool DebugRenderer::Draw()
{
	// Draw Grid Lines
	debug_line_shader_.SetActive();
	glDisable(GL_CULL_FACE);
	glBindVertexArray(VAO_grid_lines_);
	glDrawElements(GL_TRIANGLES, grid_line_indices_.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);

	// Draw Grid Arrows
	debug_grid_vel_shader_.SetActive();
	glDisable(GL_CULL_FACE);
	glBindVertexArray(VAO_grid_arrows_);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_instances_);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(0, 0); // Reuse these on each instance

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_pos_);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(1, 1); // Unique to each instance

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_colors_);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(2, 1); // Unique to each instance

	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_indices_);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(3, 1); // Unique to each instance

	glDrawArraysInstanced(GL_TRIANGLES, 0, instance_arrow_verts_.size(), arrow_positions_.size());
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);	
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);


	for (auto& view : active_views_) {
		switch (view) {
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
