#include "debug_renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

void DebugRenderer::MakeInstanceArrow(std::vector<glm::vec3>& verts) {
	glm::vec3 norm = glm::vec3(0, 0, 1);
	glm::vec3 start_pos = glm::vec3(0, 0, 0);
	glm::vec3 end_pos = glm::vec3(0, 1, 0);
	glm::vec3 arrow_head_start = glm::vec3(0, 0.95, 0.0);
	float arrow_wideness = 0.05f;
	const float k_thickness = 0.01f;

	glm::vec3 x_offset = glm::vec3(k_thickness, 0, 0);
	verts.push_back(start_pos - x_offset);
	verts.push_back(start_pos + x_offset);
	verts.push_back(end_pos + x_offset);
	verts.push_back(end_pos - x_offset);
	verts.push_back(end_pos + x_offset);
	verts.push_back(start_pos - x_offset);

	glm::vec3 z_offset = glm::vec3(0, 0, k_thickness);
	verts.push_back(start_pos - x_offset + z_offset);
	verts.push_back(start_pos + x_offset + z_offset);
	verts.push_back(end_pos + x_offset + z_offset);
	verts.push_back(end_pos - x_offset + z_offset);
	verts.push_back(end_pos + x_offset + z_offset);
	verts.push_back(start_pos - x_offset + z_offset);


}

void ConstructLineMat(const glm::vec3& scale, glm::mat4& res, const glm::vec3& start, const glm::vec3& end, const glm::vec3& right) {
	res = glm::lookAt(glm::vec3(0,0,0), right, end - start) * glm::translate(glm::mat4(1.0f), start) * glm::scale(glm::mat4(1.0f), scale);
}

void DebugRenderer::UpdateGridLines()
{
	static const glm::vec3 grid_line_color = glm::vec3(156.0 / 256.0, 158.0 / 256.0, 136.0 / 256.0);
	int grid_size = (ws_grid_upper_bound_.x - ws_grid_lower_bound_.x) / ws_grid_cell_size_ + 1;
	const float k_line_thickness = 0.01f;
	glm::vec3 line_scale = glm::vec3(k_line_thickness, ws_grid_cell_size_ / 3.0, k_line_thickness);

	std::vector<glm::mat4> line_mats;
	std::vector<glm::vec3> line_color;
	for (float z = ws_grid_lower_bound_.z; z <= ws_grid_upper_bound_.z; z += ws_grid_cell_size_) {
		for (float y = ws_grid_lower_bound_.y; y <= ws_grid_upper_bound_.y; y += ws_grid_cell_size_) {
			for (float x = ws_grid_lower_bound_.x; x <= ws_grid_upper_bound_.x; x += ws_grid_cell_size_) {
				glm::mat4 mat;
				if (x + 0.01f < ws_grid_upper_bound_.x) {
					ConstructLineMat(line_scale, mat, glm::vec3(x, y, z), glm::vec3(x + ws_grid_cell_size_, y, z), glm::vec3(0, 0, 1));
					line_mats.push_back(mat);
					line_color.push_back(grid_line_color);
				}

				if (y + 0.01f < ws_grid_upper_bound_.y) {
					ConstructLineMat(line_scale, mat, glm::vec3(x, y, z), glm::vec3(x, y + ws_grid_cell_size_, z), glm::vec3(1, 0, 0));
					line_mats.push_back(mat);
					line_color.push_back(grid_line_color);
				}
				ConstructLineMat(line_scale, mat, glm::vec3(x, y, z), glm::vec3(x, y, z + ws_grid_cell_size_), glm::vec3(0, 1, 0));
				line_mats.push_back(mat);
				line_color.push_back(grid_line_color);
			}
		}
	}

	glBindVertexArray(VAO_grid_lines_);
	// load data into vertex buffers
	grid_line_elements_ = line_mats.size();
	glBindBuffer(GL_ARRAY_BUFFER, VBO_line_mats_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_LINES * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, line_mats.size() * sizeof(glm::mat4), (void*)&line_mats[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line_color_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_LINES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, line_color.size() * sizeof(glm::vec3), (void*)&line_color[0]);

	glBindVertexArray(0);
}

void DebugRenderer::SetupOriginBuffers() {
	glGenVertexArrays(1, &VAO_origin_lines_);
	glGenBuffers(1, &VBO_origin_line_instance_);
	glGenBuffers(1, &VBO_origin_line_mats_);
	glGenBuffers(1, &VBO_origin_line_colors_);

	glBindVertexArray(VAO_origin_lines_);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_origin_line_instance_);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(glm::vec3), &k_line_instance_verts_[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	const std::vector<glm::vec3> k_end_points = { glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1) };
	const std::vector<glm::vec3> k_ups = { glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0) };

	const glm::vec3 k_origin_line_scale = glm::vec3(0.1, 1.0, 0.1);
	std::vector<glm::mat4> transforms;
	for (int i = 0; i < k_end_points.size(); i++) {
		glm::mat4 mat;
		ConstructLineMat(k_origin_line_scale, mat, glm::vec3(0,0,0), k_end_points[i], k_ups[i]);
		transforms.push_back(mat);
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO_origin_line_colors_);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(glm::vec3), &k_end_points[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_origin_line_mats_);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(glm::mat4), &transforms[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(0, 0); // Reuse on each instance
	glVertexAttribDivisor(1, 1); // Unique to each instance
	glVertexAttribDivisor(2, 1); // Unique ...
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glBindVertexArray(0);
}

void DebugRenderer::SetupGridLineBuffers() {
	// Setup for the grid lines
	glGenVertexArrays(1, &VAO_grid_lines_);
	glGenBuffers(1, &VBO_line_instance_);
	glGenBuffers(1, &VBO_line_mats_);
	glGenBuffers(1, &VBO_line_color_);

	glBindVertexArray(VAO_grid_lines_);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line_instance_);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(glm::vec3), &k_line_instance_verts_[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line_color_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_LINES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line_mats_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_LINES * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(0, 0); // Reuse on each instance
	glVertexAttribDivisor(1, 1); // Unique to each instance
	glVertexAttribDivisor(2, 1); // Unique ...
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glBindVertexArray(0);
}

void DebugRenderer::SetupGridVelocityBuffers() {
	// Setup for the arrows
	glGenVertexArrays(1, &VAO_grid_arrows_);
	glGenBuffers(1, &VBO_grid_arrow_instances_);
	glGenBuffers(1, &VBO_grid_arrow_mats_);
	glGenBuffers(1, &VBO_grid_arrow_colors_);


	std::vector<glm::vec3> instance_arrow_verts;
	MakeInstanceArrow(instance_arrow_verts);
	grid_arrow_instance_num_ = instance_arrow_verts.size();

	glBindVertexArray(VAO_grid_arrows_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_instances_);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(glm::vec3), &k_line_instance_verts_[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_colors_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_mats_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(0, 0); // Reuse on each instance
	glVertexAttribDivisor(1, 1); // Unique to each instance
	glVertexAttribDivisor(2, 1); // Unique ...
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glBindVertexArray(0);
}

DebugRenderer::DebugRenderer() :
	debug_line_shader_("debug/line.vert", "debug/line.frag"),
	debug_grid_vel_shader_("debug/grid_arrows.vert", "debug/grid_arrows.frag"),
	debug_particle_shader_("debug/particle.vert", "debug/particle.frag"),
	ws_grid_cell_size_(1.0),
	ws_grid_lower_bound_(glm::vec3(-1, -1, -1)),
	ws_grid_upper_bound_(glm::vec3(1, 1, 1)),
	cached_view_(glm::mat4()),
	cached_proj_(glm::mat4()),
	VAO_grid_lines_(0),
	VBO_line_instance_(0),
	VBO_line_mats_(0),
	VBO_line_color_(0),
	grid_line_elements_(0),
	VBO_grid_arrow_instances_(0),
	VBO_grid_arrow_mats_(0),
	VBO_grid_arrow_colors_(0),
	grid_arrow_elements_(0),
	frame_time_display_("0.0 ms/frame")
{
	SetupOriginBuffers();
	SetupGridLineBuffers();
	SetupGridVelocityBuffers();

	ToggleDebugView(GRID);
}

DebugRenderer::~DebugRenderer()
{
	glDeleteBuffers(1, &VAO_grid_lines_);
	glDeleteBuffers(1, &VBO_line_instance_);
	glDeleteBuffers(1, &VBO_line_mats_);
	glDeleteBuffers(1, &VBO_line_color_);

	glDeleteBuffers(1, &VAO_grid_arrows_);
	glDeleteBuffers(1, &VBO_grid_arrow_instances_);
	glDeleteBuffers(1, &VBO_grid_arrow_mats_);
	glDeleteBuffers(1, &VBO_grid_arrow_colors_);
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

bool DebugRenderer::SetGridVelocities(Texture3D& grid_velocities)
{
	//vel_texture_dimensions_ = grid_velocities.GetDimensions();
	//debug_grid_vel_shader_.SetUniformTexture3D("velocities", grid_velocities, GL_TEXTURE0);
	return false;
}

bool DebugRenderer::SetGridVelocities(const std::vector<glm::vec3>& grid_velocities, const unsigned int grid_dimensions)
{
	std::vector<glm::mat4> arrow_mats;
	std::vector<glm::vec3> arrow_colors;

	const glm::vec3 k_x_color = glm::vec3(0.8, 0.0, 0.0);
	const glm::vec3 k_y_color = glm::vec3(0.0, 0.8, 0.0);
	const glm::vec3 k_z_color = glm::vec3(0.0, 0.0, 0.8);
	arrow_mats.push_back(glm::mat4(1.0f));
	arrow_colors.push_back(glm::vec3(1, 0.23, .523));
	for (int z = 0; z < grid_dimensions; z++) {
		for (int y = 0; y < grid_dimensions; y++) {
			for (int x = 0; x < grid_dimensions; x++) {
				glm::vec3 x_pos = ws_grid_lower_bound_ + glm::vec3(x, y, z) * ws_grid_cell_size_ + glm::vec3(ws_grid_cell_size_ / 2.0, 0.0, 0.0);
				glm::vec3 y_pos = ws_grid_lower_bound_ + glm::vec3(x, y, z) * ws_grid_cell_size_ + glm::vec3(0.0, ws_grid_cell_size_ / 2.0, 0.0);
				glm::vec3 z_pos = ws_grid_lower_bound_ + glm::vec3(x, y, z) * ws_grid_cell_size_ + glm::vec3(0.0, 0.0, ws_grid_cell_size_ / 2.0);


				glm::vec3 sample_vel = grid_velocities[x + y * grid_dimensions + z * grid_dimensions * grid_dimensions];

				glm::vec3 x_end = x_pos + sample_vel.x;
				glm::vec3 y_end = y_pos + sample_vel.y;
				glm::vec3 z_end = z_pos + sample_vel.z;
				printf("Placing x vel vector at:\n   pos: [%3.3f, %3.3f, %3.3f]\n   vel: [%3.3f, %3.3f, %3.3f]\n   end_pos: [%3.3f, %3.3f, %3.3f]\n", x_pos.x, x_pos.y, x_pos.z, sample_vel.x, sample_vel.y, sample_vel.z, x_end.x, x_end.y, x_end.z);
				printf("Placing y vel vector at:\n   pos: [%3.3f, %3.3f, %3.3f]\n   vel: [%3.3f, %3.3f, %3.3f]\n   end_pos: [%3.3f, %3.3f, %3.3f]\n", y_pos.x, y_pos.y, y_pos.z, sample_vel.x, sample_vel.y, sample_vel.z, y_end.x, y_end.y, y_end.z);
				printf("Placing z vel vector at:\n   pos: [%3.3f, %3.3f, %3.3f]\n   vel: [%3.3f, %3.3f, %3.3f]\n   end_pos: [%3.3f, %3.3f, %3.3f]\n", z_pos.x, z_pos.y, z_pos.z, sample_vel.x, sample_vel.y, sample_vel.z, z_end.x, z_end.y, z_end.z);

				glm::mat4 x_mat;
				glm::vec3 x_scale = glm::vec3(1.0, sample_vel.x, 1.0);
				ConstructLineMat(x_scale, x_mat, x_pos, x_end, glm::vec3(0, 0, 1));

				glm::mat4 y_mat;
				glm::vec3 y_scale = glm::vec3(1.0, 1.0, 1.0);
				ConstructLineMat(y_scale, y_mat, y_pos, y_end, glm::vec3(1, 0, 0));

				glm::mat4 z_mat;
				glm::vec3 z_scale = glm::vec3(1.0, sample_vel.z, 1.0);
				ConstructLineMat(z_scale, z_mat, z_pos, z_end, glm::vec3(0, 1, 0));

				arrow_mats.push_back(x_mat);
				arrow_mats.push_back(y_mat);
				arrow_mats.push_back(z_mat);
				arrow_colors.push_back(k_x_color);
				arrow_colors.push_back(k_y_color);
				arrow_colors.push_back(k_z_color);

				if (x + 1 == grid_dimensions) {
					// Add the last two vectors
					y_pos = ws_grid_lower_bound_ + glm::vec3(x, y + 1, z) * ws_grid_cell_size_ + glm::vec3(0.0, ws_grid_cell_size_ / 2.0, 0.0);
					z_pos = ws_grid_lower_bound_ + glm::vec3(x, y, z + 1) * ws_grid_cell_size_ + glm::vec3(0.0, 0.0, ws_grid_cell_size_ / 2.0);
					sample_vel = grid_velocities[(x + 1) + y * grid_dimensions + z * grid_dimensions * grid_dimensions];
					y_end = y_pos + sample_vel.y;
					z_end = z_pos + sample_vel.z;

					y_scale = glm::vec3(1.0, sample_vel.y, 1.0);
					ConstructLineMat(y_scale, y_mat, y_pos, y_end, glm::vec3(1, 0, 0));

					z_scale = glm::vec3(1.0, sample_vel.z, 1.0);
					ConstructLineMat(z_scale, z_mat, z_pos, z_end, glm::vec3(0, 1, 0));

					arrow_mats.push_back(y_mat);
					arrow_mats.push_back(z_mat);

					arrow_colors.push_back(k_y_color);
					arrow_colors.push_back(k_z_color);
				}
				if (y + 1 == grid_dimensions) {
					// Add the last two vectors
					x_pos = ws_grid_lower_bound_ + glm::vec3(x + 1, y, z) * ws_grid_cell_size_ + glm::vec3(ws_grid_cell_size_ / 2.0, 0.0, 0.0);
					z_pos = ws_grid_lower_bound_ + glm::vec3(x, y, z + 1) * ws_grid_cell_size_ + glm::vec3(0.0, 0.0, ws_grid_cell_size_ / 2.0);
					sample_vel = grid_velocities[x + (y + 1) * grid_dimensions + z * grid_dimensions * grid_dimensions];
					x_end = x_pos + sample_vel.x;
					z_end = z_pos + sample_vel.z;

					x_scale = glm::vec3(1.0, sample_vel.x, 1.0);
					ConstructLineMat(x_scale, x_mat, x_pos, x_end, glm::vec3(0, 0, 1));

					z_scale = glm::vec3(1.0, sample_vel.z, 1.0);
					ConstructLineMat(z_scale, z_mat, z_pos, z_end, glm::vec3(0, 1, 0));

					arrow_mats.push_back(x_mat);
					arrow_mats.push_back(z_mat);

					arrow_colors.push_back(k_x_color);
					arrow_colors.push_back(k_z_color);
				}
				if (z + 1 == grid_dimensions) {
					// Add the last two vectors
					x_pos = ws_grid_lower_bound_ + glm::vec3(x + 1, y, z) * ws_grid_cell_size_ + glm::vec3(ws_grid_cell_size_ / 2.0, 0.0, 0.0);
					y_pos = ws_grid_lower_bound_ + glm::vec3(x, y + 1, z) * ws_grid_cell_size_ + glm::vec3(0.0, ws_grid_cell_size_ / 2.0, 0.0);
					sample_vel = grid_velocities[x + y * grid_dimensions + (z + 1) * grid_dimensions * grid_dimensions];
					x_end = x_pos + sample_vel.x;
					y_end = y_pos + sample_vel.y;

					x_scale = glm::vec3(1.0, sample_vel.x, 1.0);
					ConstructLineMat(x_scale, x_mat, x_pos, x_end, glm::vec3(0, 0, 1));

					y_scale = glm::vec3(1.0, sample_vel.y, 1.0);
					ConstructLineMat(y_scale, y_mat, y_pos, y_end, glm::vec3(1, 0, 0));

					arrow_mats.push_back(x_mat);
					arrow_mats.push_back(y_mat);

					arrow_colors.push_back(k_x_color);
					arrow_colors.push_back(k_y_color);
				}
			}
		}
	}


	glBindVertexArray(VAO_grid_arrows_);
	// load data into vertex buffers
	grid_arrow_elements_ = arrow_mats.size();
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_mats_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, arrow_mats.size() * sizeof(glm::mat4), (void*)&arrow_mats[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_colors_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, arrow_colors.size() * sizeof(glm::vec3), (void*)&arrow_colors[0]);

	glBindVertexArray(0);


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
			debug_line_shader_.SetActive();
			glBindVertexArray(VAO_origin_lines_);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 3);
			glBindVertexArray(0);
			break;
		case GRID:
			// Draw Grid Lines
			debug_line_shader_.SetActive();
			glBindVertexArray(VAO_grid_lines_);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 36, grid_line_elements_);
			glBindVertexArray(0);
			break;
		case GRID_VELOCITIES:
			debug_line_shader_.SetActive();
			glBindVertexArray(VAO_grid_arrows_);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 36, grid_arrow_elements_);
			glBindVertexArray(0);
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
