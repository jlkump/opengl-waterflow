#include "debug_renderer.hpp"

bool DebugRenderer::UpdateGridLines()
{
	int grid_size = (ws_grid_upper_bound_.x - ws_grid_lower_bound_.x) / ws_grid_cell_size_ + 1;
	glm::vec3 grid_line_color = glm::vec3(156, 158, 136);


	grid_line_vertices_.clear();
	grid_line_indices_.clear();

	int z_idx = 0;
	for (float z = ws_grid_lower_bound_.z; z <= ws_grid_upper_bound_.z; z += ws_grid_cell_size_) {
		int y_idx = 0;
		for (float y = ws_grid_lower_bound_.y; y <= ws_grid_upper_bound_.y; y += ws_grid_cell_size_) {
			int x_idx = 0;
			for (float x = ws_grid_lower_bound_.x; x <= ws_grid_upper_bound_.x; x += ws_grid_cell_size_) {
				grid_line_vertices_.push_back(DebugLineVert(glm::vec3(x, y, z), grid_line_color));
				if (x_idx + 1 < grid_size) {
					grid_line_indices_.push_back(z_idx * grid_size * grid_size + y_idx * grid_size + x_idx);
					grid_line_indices_.push_back(z_idx * grid_size * grid_size + y_idx * grid_size + x_idx + 1);
				}
				if (y_idx + 1 < grid_size) {
					grid_line_indices_.push_back(z_idx * grid_size * grid_size + y_idx * grid_size + x_idx);
					grid_line_indices_.push_back(z_idx * grid_size * grid_size + (y_idx + 1) * grid_size + x_idx);
				}
				if (z_idx + 1 < grid_size) {
					grid_line_indices_.push_back(z_idx * grid_size * grid_size + y_idx * grid_size + x_idx);
					grid_line_indices_.push_back((z_idx + 1) * grid_size * grid_size + y_idx * grid_size + x_idx);
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

DebugRenderer::DebugRenderer() :
	debug_line_shader_("debug/line.vert", "debug/line.frag"),
	debug_particle_shader_("debug/particle.vert","debug/particle.frag"),
	ws_grid_cell_size_(1.0),
	ws_grid_lower_bound_(glm::vec3(-1, -1, -1)),
	ws_grid_upper_bound_(glm::vec3(1, 1, 1)),
	cached_view_(glm::mat4()),
	cached_proj_(glm::mat4()),
	VAO_grid_lines_(0),
	VBO_grid_lines_(0),
	EBO_grid_lines_(0),
	VAO_grid_arrows_(0),
	VBO_grid_arrows_(0),
	EBO_grid_arrows_(0)
{
	glGenVertexArrays(1, &VAO_grid_lines_);
	glGenBuffers(1, &VBO_grid_lines_);
	glGenBuffers(1, &EBO_grid_lines_);

	glGenVertexArrays(1, &VAO_grid_arrows_);
	glGenBuffers(1, &VBO_grid_arrows_);
	glGenBuffers(1, &EBO_grid_arrows_);

	UpdateGridLines();
}

DebugRenderer::~DebugRenderer()
{
	glDeleteBuffers(1, &VAO_grid_lines_);
	glDeleteBuffers(1, &VBO_grid_lines_);
	glDeleteBuffers(1, &EBO_grid_lines_);

	glDeleteBuffers(1, &VAO_grid_arrows_);
	glDeleteBuffers(1, &VBO_grid_arrows_);
	glDeleteBuffers(1, &EBO_grid_arrows_);
}

bool DebugRenderer::SetGridBoundaries(const glm::vec3& lower_left, const glm::vec3& upper_right)
{
	ws_grid_lower_bound_ = lower_left;
	ws_grid_upper_bound_ = upper_right;
	UpdateGridLines();
	return true;
}

bool DebugRenderer::SetGridCellInterval(const float cell_size)
{
	ws_grid_cell_size_ = cell_size;
	UpdateGridLines();
	return true;
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
	debug_line_shader_.SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	// TODO: Update uniforms for those shaders who need it
	return true;
}

bool DebugRenderer::SetProjectionMat(const glm::mat4 proj)
{
	cached_proj_ = proj;
	debug_line_shader_.SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	// TODO: Update uniforms for those shaders who need it
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
	glLineWidth(10.0f);
	glBindVertexArray(VAO_grid_lines_);
	glDrawElements(GL_LINES, grid_line_indices_.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glLineWidth(1.0f);

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
