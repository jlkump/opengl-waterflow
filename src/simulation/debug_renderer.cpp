#include "debug_renderer.hpp"
#include "debug_renderer.hpp"
#include "debug_renderer.hpp"

// Only called once really since we instance multiple of a single arrow's vertices
bool DebugRenderer::MakeArrow(std::vector<glm::vec3>& verts, float thickness)
{
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

	verts.push_back(end_pos + glm::vec3(0, 1, 0) * thickness*2.0f);
	verts.push_back(end_pos);
	verts.push_back(arrow_head_start + glm::normalize(thickness_dir) * arrow_wideness);
	verts.push_back(arrow_head_start + glm::normalize(thickness_dir) * arrow_wideness + thickness_dir*2.0f);
	// inefficent slightly
	verts.push_back(arrow_head_start + glm::normalize(thickness_dir) * arrow_wideness);
	verts.push_back(end_pos + glm::vec3(0, 1, 0) * thickness*2.0f);

	verts.push_back(end_pos + glm::vec3(0, 1, 0) * thickness*2.0f);
	verts.push_back(end_pos);
	verts.push_back(arrow_head_start - glm::normalize(thickness_dir) * arrow_wideness);
	verts.push_back(arrow_head_start - glm::normalize(thickness_dir) * arrow_wideness - thickness_dir*2.0f);
	// Inefficent slightly
	verts.push_back(arrow_head_start - glm::normalize(thickness_dir) * arrow_wideness);
	verts.push_back(end_pos + glm::vec3(0, 1, 0) * thickness*2.0f);

	return false;
}

void DebugRenderer::UpdateGridLines()
{
	static const glm::vec3 grid_line_color = glm::vec3(156.0 / 256.0, 158.0 / 256.0, 136.0 / 256.0);
	int grid_size = (ws_grid_upper_bound_.x - ws_grid_lower_bound_.x) / ws_grid_cell_size_ + 1;

	std::vector<glm::vec3> line_pos;
	std::vector<glm::vec3> line_end;
	std::vector<glm::vec3> line_color;
	int z_idx = 0;
	int index = 0;
	for (float z = ws_grid_lower_bound_.z; z <= ws_grid_upper_bound_.z; z += ws_grid_cell_size_) {
		int y_idx = 0;
		for (float y = ws_grid_lower_bound_.y; y <= ws_grid_upper_bound_.y; y += ws_grid_cell_size_) {
			int x_idx = 0;
			for (float x = ws_grid_lower_bound_.x; x <= ws_grid_upper_bound_.x; x += ws_grid_cell_size_) {
				if (x_idx + 1 < grid_size) {
					line_pos.push_back(glm::vec3(x, y, z));
					line_end.push_back(glm::vec3(x + ws_grid_cell_size_, y, z));
					line_color.push_back(grid_line_color);
				}
				if (y_idx + 1 < grid_size) {
					line_pos.push_back(glm::vec3(x, y, z));
					line_end.push_back(glm::vec3(x, y + ws_grid_cell_size_, z));
					line_color.push_back(grid_line_color);
				}
				if (z_idx + 1 < grid_size) {
					line_pos.push_back(glm::vec3(x, y, z));
					line_end.push_back(glm::vec3(x, y, z + ws_grid_cell_size_));
					line_color.push_back(grid_line_color);
				}
				x_idx++;
			}
			y_idx++;
		}
		z_idx++;
	}

	glBindVertexArray(VAO_grid_lines_);
	// load data into vertex buffers
	grid_line_elements_ = line_pos.size();
	glBindBuffer(GL_ARRAY_BUFFER, VBO_line_pos_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_LINES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, line_pos.size() * sizeof(glm::vec3), (void*)&line_pos[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line_end_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_LINES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, line_end.size() * sizeof(glm::vec3), (void*)&line_end[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line_color_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_LINES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, line_color.size() * sizeof(glm::vec3), (void*)&line_color[0]);

	glBindVertexArray(0);
}

void DebugRenderer::UpdateArrowPositions()
{
	std::vector<glm::vec3> arrow_positions;
	std::vector<glm::vec3> arrow_colors;
	std::vector<glm::vec3> arrow_indexes;

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
					arrow_positions.push_back(glm::vec3(x + offset, y, z));
					arrow_colors.push_back(glm::vec3(1, 0, 0));
					arrow_indexes.push_back(glm::vec3(
						(x - ws_grid_lower_bound_.x) / grid_side_size, 
						(y - ws_grid_lower_bound_.y) / grid_side_size, 
						(z - ws_grid_lower_bound_.z) / grid_side_size));
				}
				if (y_idx + 1 < grid_size) {
					arrow_positions.push_back(glm::vec3(x, y + offset, z));
					arrow_colors.push_back(glm::vec3(0, 1, 0));
					arrow_indexes.push_back(glm::vec3(
						(x - ws_grid_lower_bound_.x) / grid_side_size,
						(y - ws_grid_lower_bound_.y) / grid_side_size, 
						(z - ws_grid_lower_bound_.z) / grid_side_size));
				}
				if (z_idx + 1 < grid_size) {
					arrow_positions.push_back(glm::vec3(x, y, z + offset));
					arrow_colors.push_back(glm::vec3(0, 0, 1));
					arrow_indexes.push_back(glm::vec3(
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
	//printf("Arrow verts:\n");
	//for (auto& v : arrow_positions_) {
	//	printf("(%4.4f, %4.4f, %4.4f)\n", v.x, v.y, v.z);
	//}

	grid_arrow_elements_ = arrow_positions.size();
	glBindVertexArray(VAO_grid_arrows_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_pos_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, arrow_positions.size() * sizeof(glm::vec3), (void*)&arrow_positions[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_colors_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, arrow_colors.size() * sizeof(glm::vec3), (void*)&arrow_colors[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_indices_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, arrow_indexes.size() * sizeof(glm::vec3), (void*)&arrow_indexes[0]);

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
	VBO_line_instance_(0),
	VBO_line_pos_(0),
	VBO_line_end_(0),
	VBO_line_color_(0),
	grid_line_elements_(0),
	VBO_grid_arrow_instances_(0),
	VBO_grid_arrow_indices_(0),
	VBO_grid_arrow_pos_(0),
	VBO_grid_arrow_colors_(0),
	grid_arrow_elements_(0),
	frame_time_display_("0.0 ms/frame")
{
	// Setup for the grid lines
	glGenVertexArrays(1, &VAO_grid_lines_);
	glGenBuffers(1, &VBO_line_instance_);
	glGenBuffers(1, &VBO_line_pos_);
	glGenBuffers(1, &VBO_line_end_);
	glGenBuffers(1, &VBO_line_color_);

	static const GLfloat line_instance_verts[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	}; // 4 vertices, triangle strip

	glBindVertexArray(VAO_grid_lines_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_line_instance_);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), line_instance_verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_line_pos_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_LINES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_line_end_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_LINES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_line_color_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_LINES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBindVertexArray(0);

	debug_line_shader_.SetUniform1fv("line_thickness", 0.01);

	// Setup for the arrows
	glGenVertexArrays(1, &VAO_grid_arrows_);
	glGenBuffers(1, &VBO_grid_arrow_instances_);
	glGenBuffers(1, &VBO_grid_arrow_pos_);
	glGenBuffers(1, &VBO_grid_arrow_colors_);
	glGenBuffers(1, &VBO_grid_arrow_indices_);


	std::vector<glm::vec3> instance_arrow_verts;
	MakeArrow(instance_arrow_verts, 0.01);

	glBindVertexArray(VAO_grid_arrows_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_instances_);
	glBufferData(GL_ARRAY_BUFFER, instance_arrow_verts.size() * sizeof(glm::vec3), &instance_arrow_verts[0], GL_STATIC_DRAW);
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
	glDeleteBuffers(1, &VBO_line_instance_);
	glDeleteBuffers(1, &VBO_line_pos_);
	glDeleteBuffers(1, &VBO_line_end_);
	glDeleteBuffers(1, &VBO_line_color_);

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

bool DebugRenderer::SetView(const glm::mat4& view)
{
	cached_view_ = view;
	// Update the uniforms for shaders
	debug_line_shader_.SetUniformMatrix4fv("view", cached_view_);
	debug_line_shader_.SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	debug_grid_vel_shader_.SetUniformMatrix4fv("view", cached_view_);
	debug_grid_vel_shader_.SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	return true;
}

bool DebugRenderer::SetProjection(const glm::mat4& proj)
{
	cached_proj_ = proj;
	// Update the uniforms for shaders
	debug_line_shader_.SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	debug_grid_vel_shader_.SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	return true;
}

void DebugRenderer::ToggleDebugView(DebugView view_toggle)
{
	if (active_views_.count(view_toggle) != 0) {
		active_views_.erase(view_toggle);
	} else {
		active_views_.insert(view_toggle);
	}
}

bool DebugRenderer::Draw()
{
	for (auto& view : active_views_) {
		switch (view) {
		case ORIGIN:

			break;
		case GRID:
			// Draw Grid Lines
			printf("Drawing grid with elements : %d\n", grid_line_elements_);
			debug_line_shader_.SetActive();
			glDisable(GL_CULL_FACE);
			glBindVertexArray(VAO_grid_lines_);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_line_instance_);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glVertexAttribDivisor(0, 0); // Reuse these on each instance

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_line_pos_);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glVertexAttribDivisor(1, 1); // Unique to each instance

			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_line_end_);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glVertexAttribDivisor(2, 1); // Unique to each instance

			glEnableVertexAttribArray(3);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_line_color_);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glVertexAttribDivisor(3, 1); // Unique to each instance

			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, grid_line_elements_);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);
			glBindVertexArray(0);
			glEnable(GL_CULL_FACE);
			break;
		case GRID_VELOCITIES:
			// Draw Grid Arrows
			//debug_grid_vel_shader_.SetActive();
			//glDisable(GL_CULL_FACE);
			//glBindVertexArray(VAO_grid_arrows_);
			//glEnableVertexAttribArray(0);
			//glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_instances_);
			//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			//glVertexAttribDivisor(0, 0); // Reuse these on each instance

			//glEnableVertexAttribArray(1);
			//glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_pos_);
			//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			//glVertexAttribDivisor(1, 1); // Unique to each instance

			//glEnableVertexAttribArray(2);
			//glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_colors_);
			//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			//glVertexAttribDivisor(2, 1); // Unique to each instance

			//glEnableVertexAttribArray(3);
			//glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_indices_);
			//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			//glVertexAttribDivisor(3, 1); // Unique to each instance

			//glDrawArraysInstanced(GL_TRIANGLES, 0, 18, grid_arrow_elements_);
			//glDisableVertexAttribArray(0);
			//glDisableVertexAttribArray(1);
			//glDisableVertexAttribArray(2);
			//glDisableVertexAttribArray(3);
			//glBindVertexArray(0);
			//glEnable(GL_CULL_FACE);
			break;
		case GRID_DYE:
			break;
		case PARTICLES:
			break;
		case PARTICLE_VELOCITIES:
			break;
		case FRAME_TIME:
			frame_time_display_.Draw();
			break;
		}
	}

	return true;
}

void DebugRenderer::UpdateFrameTime(float frame_time) {
	frame_time_display_.SetText(std::to_string(frame_time) + " ms/frame");
}
