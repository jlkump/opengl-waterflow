#include "debug_renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "sequential_simulation.hpp"
#include "print_debug_helpers.h"

void DebugRenderer::MakeInstanceArrow(std::vector<glm::vec3>& verts) {
	float arrow_head_start = 0.95f;
	float line_thickness_relative = 0.8f;
	float arrow_wideness = 1.5;
	for (int i = 0; i < 36; i++) {
		verts.push_back(glm::vec3(
			((k_cube_verts[i * 3] * 2.0f) - 1.0f) * line_thickness_relative, 
			k_cube_verts[i * 3 + 1] * arrow_head_start, 
			((k_cube_verts[i * 3 + 2] * 2.0f) - 1.0f) * line_thickness_relative));
	}
	verts.push_back(glm::vec3(-arrow_wideness, arrow_head_start, -arrow_wideness));
	verts.push_back(glm::vec3(-arrow_wideness, arrow_head_start, arrow_wideness));
	verts.push_back(glm::vec3(0, 1, 0));

	verts.push_back(glm::vec3(-arrow_wideness, arrow_head_start, -arrow_wideness));
	verts.push_back(glm::vec3(arrow_wideness, arrow_head_start, -arrow_wideness));
	verts.push_back(glm::vec3(0, 1, 0));

	verts.push_back(glm::vec3(-arrow_wideness, arrow_head_start, arrow_wideness));
	verts.push_back(glm::vec3(arrow_wideness, arrow_head_start, arrow_wideness));
	verts.push_back(glm::vec3(0, 1, 0));

	verts.push_back(glm::vec3(arrow_wideness, arrow_head_start, -arrow_wideness));
	verts.push_back(glm::vec3(arrow_wideness, arrow_head_start, arrow_wideness));
	verts.push_back(glm::vec3(0, 1, 0));

	grid_arrow_instance_num_ = verts.size();
}

void ConstructAxisAlignedModelMat(glm::mat4& res, const glm::vec3& scale, const glm::vec3& start) {
	res = glm::scale(glm::translate(glm::mat4(1.0f), start), scale);
}

void ConstructVectorAlignedModelMat(glm::mat4& res, const glm::vec3& scale, const glm::vec3& pos, const glm::vec3& forward, const glm::vec3& up, const glm::vec3& right) {
	glm::mat4 rot_mat = glm::mat4(
		right.x, right.y, right.z, 0.0,
		up.x, up.y, up.z, 0.0,
		forward.x, forward.y, forward.z, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
	
	res = glm::translate(glm::mat4(1.0f), pos) * rot_mat * glm::scale(glm::mat4(1.0f), scale);
}

void DebugRenderer::UpdateGridLines()
{
	static const glm::vec3 grid_line_color = glm::vec3(156.0 / 256.0, 158.0 / 256.0, 136.0 / 256.0);
	int grid_size = (ws_grid_upper_bound_.x - ws_grid_lower_bound_.x) / ws_grid_cell_size_ + 1;
	const float k_line_thickness = 0.01f;
	glm::vec3 line_scale = glm::vec3(k_line_thickness, ws_grid_cell_size_ / 3.0, k_line_thickness);

	//printf("Updating grid lines\n   low bound: [%3.3f, %3.3f, %3.3f]\n   upper bound: [%3.3f, %3.3f, %3.3f]\n", ws_grid_lower_bound_.x, ws_grid_lower_bound_.y, ws_grid_lower_bound_.z, ws_grid_upper_bound_.x, ws_grid_upper_bound_.y, ws_grid_upper_bound_.z);
	std::vector<glm::mat4> line_mats;
	std::vector<glm::vec3> line_color;
	for (float z = ws_grid_lower_bound_.z; z <= ws_grid_upper_bound_.z; z += ws_grid_cell_size_) {
		for (float y = ws_grid_lower_bound_.y; y <= ws_grid_upper_bound_.y; y += ws_grid_cell_size_) {
			for (float x = ws_grid_lower_bound_.x; x <= ws_grid_upper_bound_.x; x += ws_grid_cell_size_) {
				glm::mat4 mat;
				//printf("At grid ws pos: \n   [%3.3f, %3.3f, %3.3f] with upper bounds [%3.3f, %3.3f, %3.3f]\n", x, y, z, ws_grid_upper_bound_.x, ws_grid_upper_bound_.y, ws_grid_upper_bound_.z);
				if (x < ws_grid_upper_bound_.x) {
					//printf("   Drawing x line\n");
					ConstructAxisAlignedModelMat(mat, glm::vec3(ws_grid_cell_size_ / 3.0, k_line_thickness, k_line_thickness), glm::vec3(x, y, z));
					line_mats.push_back(mat);
					line_color.push_back(grid_line_color);
					//line_color.push_back(glm::vec3(1, 0, 0));
				}

				if (x > ws_grid_lower_bound_.x) {
					ConstructAxisAlignedModelMat(mat, glm::vec3(-ws_grid_cell_size_ / 3.0, k_line_thickness, k_line_thickness), glm::vec3(x, y, z));
					line_mats.push_back(mat);
					line_color.push_back(grid_line_color);
					//line_color.push_back(glm::vec3(0, 1, 1));
				}

				if (y < ws_grid_upper_bound_.y) {
					//printf("   Drawing y line\n");
					ConstructAxisAlignedModelMat(mat, glm::vec3(k_line_thickness, ws_grid_cell_size_ / 3.0, k_line_thickness), glm::vec3(x, y, z));
					line_mats.push_back(mat);
					line_color.push_back(grid_line_color);
					//line_color.push_back(glm::vec3(0, 1, 0));
				}

				if (y > ws_grid_lower_bound_.y) {
					ConstructAxisAlignedModelMat(mat, glm::vec3(k_line_thickness, -ws_grid_cell_size_ / 3.0, k_line_thickness), glm::vec3(x, y, z));
					line_mats.push_back(mat);
					line_color.push_back(grid_line_color);
					//line_color.push_back(glm::vec3(1, 0, 1));
				}

				if (z < ws_grid_upper_bound_.z) {
					//printf("   Drawing z line\n");
					ConstructAxisAlignedModelMat(mat, glm::vec3(k_line_thickness, k_line_thickness, ws_grid_cell_size_ / 3.0), glm::vec3(x, y, z));
					line_mats.push_back(mat);
					line_color.push_back(grid_line_color);
					//line_color.push_back(glm::vec3(0, 0, 1));
				}

				if (z > ws_grid_lower_bound_.z) {
					ConstructAxisAlignedModelMat(mat, glm::vec3(k_line_thickness, k_line_thickness, -ws_grid_cell_size_ / 3.0), glm::vec3(x, y, z));
					line_mats.push_back(mat);
					line_color.push_back(grid_line_color);
					//line_color.push_back(glm::vec3(1, 1, 0));
				}
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

void DebugRenderer::UpdateGridAxisVelocities(const std::vector<glm::vec3>& velocities, const unsigned int grid_dim)
{
	std::vector<glm::mat4> arrow_mats;
	std::vector<glm::vec3> arrow_colors;
	const float thickness = 0.01f;

	const glm::vec3 k_x_color = glm::vec3(0.8, 0.2, 0.2);
	const glm::vec3 k_y_color = glm::vec3(0.2, 0.8, 0.2);
	const glm::vec3 k_z_color = glm::vec3(0.2, 0.2, 0.8);
	//arrow_mats.push_back(glm::scale(glm::mat4(1.0f), glm::vec3(thickness, 1.0f, thickness)));
	//arrow_colors.push_back(glm::vec3(0.3, 0.4, 1.0));
	for (int z = 0; z < grid_dim; z++) {
		for (int y = 0; y < grid_dim; y++) {
			for (int x = 0; x < grid_dim; x++) {
				glm::vec3 x_pos = ws_grid_lower_bound_ + glm::vec3(x, y, z) * ws_grid_cell_size_ + glm::vec3(0.0, ws_grid_cell_size_ / 2.0, ws_grid_cell_size_ / 2.0);
				glm::vec3 y_pos = ws_grid_lower_bound_ + glm::vec3(x, y, z) * ws_grid_cell_size_ + glm::vec3(ws_grid_cell_size_ / 2.0, 0.0, ws_grid_cell_size_ / 2.0);
				glm::vec3 z_pos = ws_grid_lower_bound_ + glm::vec3(x, y, z) * ws_grid_cell_size_ + glm::vec3(ws_grid_cell_size_ / 2.0, ws_grid_cell_size_ / 2.0, 0.0);

				glm::vec3 sample_vel = GetVelocityFrom3DGridCell(velocities, grid_dim, x, y, z);

				glm::vec3 x_scale = glm::vec3(thickness, sample_vel.x, thickness);
				glm::vec3 y_scale = glm::vec3(thickness, sample_vel.y, thickness);
				glm::vec3 z_scale = glm::vec3(thickness, sample_vel.z, thickness);

				glm::mat4 x_mat;
				glm::mat4 y_mat;
				glm::mat4 z_mat;

				ConstructVectorAlignedModelMat(x_mat, x_scale, x_pos, glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1));
				ConstructVectorAlignedModelMat(y_mat, y_scale, y_pos, glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
				ConstructVectorAlignedModelMat(z_mat, z_scale, z_pos, glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));

				arrow_mats.push_back(x_mat);
				arrow_mats.push_back(y_mat);
				arrow_mats.push_back(z_mat);

				arrow_colors.push_back(k_x_color);
				arrow_colors.push_back(k_y_color);
				arrow_colors.push_back(k_z_color);
			}
		}
	}
	glBindVertexArray(VAO_grid_axis_arrows_);
	// load data into vertex buffers
	grid_axis_arrow_elements_ = arrow_mats.size();
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_axis_arrow_mats_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_AXIS_GRID_ARROWS * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, arrow_mats.size() * sizeof(glm::mat4), (void*)&arrow_mats[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_axis_arrow_colors_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_AXIS_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, arrow_colors.size() * sizeof(glm::vec3), (void*)&arrow_colors[0]);

	glBindVertexArray(0);
}

void DebugRenderer::UpdateGridVelocities(const std::vector<glm::vec3>& velocities, const unsigned int grid_dim)
{
	std::vector<glm::mat4> arrow_mats;
	std::vector<glm::vec3> arrow_colors;
	const float thickness = 0.1f;
	glm::vec3 k_arrow_color = glm::vec3(0.5, 0.7, 0.4);
	for (int z = 0; z < grid_dim; z++) {
		for (int y = 0; y < grid_dim; y++) {
			for (int x = 0; x < grid_dim; x++) {
				glm::vec3 pos = ws_grid_lower_bound_ + glm::vec3(x, y, z) * ws_grid_cell_size_ + glm::vec3(0.5, 0.5, 0.5) * ws_grid_cell_size_;

				glm::vec3 vel = GetVelocityFrom3DGridCell(velocities, grid_dim, x, y, z);

				//printf("Initial velocity at %d, %d, %d:\n", x, y, z);
				//PrintVec3(vel);

				glm::vec3 up = glm::normalize(vel);
				glm::vec3 right = glm::cross(glm::vec3(1, 0, 0), vel);
				glm::vec3 forward = glm::cross(right, vel);
				right = glm::cross(vel, forward);

				glm::vec3 scale = glm::vec3(thickness, glm::length(vel), thickness);

				glm::mat4 mat;
				//printf("   right:\n      ");
				//PrintVec3(right);
				//printf("   up:\n      ");
				//PrintVec3(up);
				//printf("   forward:\n      ");
				//PrintVec3(forward);
				ConstructVectorAlignedModelMat(mat, scale, pos, right, up, forward);
				//printf("   final mat:\n");
				//PrintMat4(mat);
				
				// ConstructVectorAlignedModelMat(mat, scale, pos, glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));

				arrow_mats.push_back(mat);

				arrow_colors.push_back(k_arrow_color);
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
}

void DebugRenderer::UpdateGridCells(const unsigned int grid_dim)
{
	std::vector<glm::mat4> model_mats;
	for (int x = 0; x < grid_dim; x++) {
		for (int y = 0; y < grid_dim; y++) {
			for (int z = 0; z < grid_dim; z++) {
				glm::vec3 scale = glm::vec3(ws_grid_cell_size_);
				glm::vec3 pos = glm::vec3(x, y, z) * ws_grid_cell_size_ + ws_grid_lower_bound_;
				glm::mat4 mat;
				ConstructAxisAlignedModelMat(mat, scale, pos);
				model_mats.push_back(mat);
			}
		}
	}

	glBindVertexArray(VAO_grid_cell_);
	// load data into vertex buffers
	grid_cell_elements_ = model_mats.size();
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_cell_mats_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_CELLS * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, model_mats.size() * sizeof(glm::mat4), (void*)&model_mats[0]);

	glBindVertexArray(0);
}

void DebugRenderer::UpdateGridCellFloats(const std::vector<float>& floats, const unsigned int grid_dim)
{
	std::vector<float> final_floats;
	for (int x = 0; x < grid_dim; x++) {
		for (int y = 0; y < grid_dim; y++) {
			for (int z = 0; z < grid_dim; z++) {
				final_floats.push_back(GetFloatValFrom3DGridCell(floats, grid_dim, x, y, z));
			}
		}
	}
	glBindVertexArray(VAO_grid_cell_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_cell_float_val_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_CELLS * sizeof(float), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, final_floats.size() * sizeof(float), (void*)&final_floats[0]);
	glBindVertexArray(0);
}

void DebugRenderer::SetVariableDefaults()
{
	ws_grid_lower_bound_ = glm::vec3(-1.0f, -1.0f, -1.0f);
	ws_grid_upper_bound_ = glm::vec3(1.0f, 1.0f, 1.0f);
	ws_grid_cell_size_ = 1.0f;

	active_cell_view_ = NONE;

	cached_view_ = glm::mat4(1.0f);
	cached_proj_ = glm::mat4(1.0f);

	// Origin
	VAO_origin_lines_ = 0;
	VBO_origin_line_instance_ = 0;
	VBO_origin_line_mats_ = 0;
	VBO_origin_line_colors_ = 0;

	// Grid lines 
	VAO_grid_lines_ = 0;
	VBO_line_instance_ = 0;
	VBO_line_mats_ = 0;
	VBO_line_color_ = 0;
	grid_line_elements_ = 0;

	// Grid arrows
	grid_arrow_instance_num_ = 0; // Number of verts in the arrow instance
	VAO_grid_arrows_ = 0;
	VBO_grid_arrow_instance_ = 0;
	VBO_grid_arrow_mats_ = 0;
	VBO_grid_arrow_colors_ = 0;
	grid_arrow_elements_ = 0;

	// Axis aligned grid arrows
	VAO_grid_axis_arrows_ = 0;
	VBO_grid_axis_arrow_instance_ = 0;
	VBO_grid_axis_arrow_mats_ = 0;
	VBO_grid_axis_arrow_colors_ = 0;
	grid_axis_arrow_elements_ = 0;

	// Cell fluid visualization
	VAO_grid_cell_ = 0;
	VBO_grid_cell_instance_ = 0;
	VBO_grid_cell_mats_ = 0;
	VBO_grid_cell_float_val_ = 0;
	grid_cell_elements_ = 0;
}

void DebugRenderer::SetupOriginBuffers() {
	glGenVertexArrays(1, &VAO_origin_lines_);
	glGenBuffers(1, &VBO_origin_line_instance_);
	glGenBuffers(1, &VBO_origin_line_mats_);
	glGenBuffers(1, &VBO_origin_line_colors_);

	glBindVertexArray(VAO_origin_lines_);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_origin_line_instance_);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(glm::vec3), &k_cube_verts[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	const std::vector<glm::vec3> k_end_points = { glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1) };
	const std::vector<glm::vec3> k_origin_scales = { glm::vec3(1.0, 0.1, 0.1), glm::vec3(0.1, 1.0, 0.1), glm::vec3(0.1, 0.1, 1.0) };

	std::vector<glm::mat4> transforms;
	for (int i = 0; i < k_end_points.size(); i++) {
		glm::mat4 mat;
		ConstructAxisAlignedModelMat(mat, k_origin_scales[i], glm::vec3(0, 0, 0));
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
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(glm::vec3), &k_cube_verts[0], GL_STATIC_DRAW);
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

void DebugRenderer::SetupGridVelocityBuffers() 
{
	// Setup for the arrows
	glGenVertexArrays(1, &VAO_grid_arrows_);
	glGenBuffers(1, &VBO_grid_arrow_instance_);
	glGenBuffers(1, &VBO_grid_arrow_colors_);
	glGenBuffers(1, &VBO_grid_arrow_mats_);


	std::vector<glm::vec3> instance_arrow_verts;
	MakeInstanceArrow(instance_arrow_verts);

	glBindVertexArray(VAO_grid_arrows_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_arrow_instance_);
	glBufferData(GL_ARRAY_BUFFER, instance_arrow_verts.size() * sizeof(glm::vec3), &instance_arrow_verts[0], GL_STATIC_DRAW);
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


void DebugRenderer::SetupGridAxisVelocityBuffers()
{
	// Setup for the arrows
	glGenVertexArrays(1, &VAO_grid_axis_arrows_);
	glGenBuffers(1, &VBO_grid_axis_arrow_instance_);
	glGenBuffers(1, &VBO_grid_axis_arrow_colors_);
	glGenBuffers(1, &VBO_grid_axis_arrow_mats_);


	std::vector<glm::vec3> instance_arrow_verts;
	MakeInstanceArrow(instance_arrow_verts);

	glBindVertexArray(VAO_grid_axis_arrows_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_axis_arrow_instance_);
	glBufferData(GL_ARRAY_BUFFER, instance_arrow_verts.size() * sizeof(glm::vec3), &instance_arrow_verts[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_axis_arrow_colors_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_AXIS_GRID_ARROWS * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_axis_arrow_mats_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_AXIS_GRID_ARROWS * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
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

void DebugRenderer::SetupGridCellBuffers()
{
	// Setup for the arrows
	glGenVertexArrays(1, &VAO_grid_cell_);
	glGenBuffers(1, &VBO_grid_cell_instance_);
	glGenBuffers(1, &VBO_grid_cell_float_val_);
	glGenBuffers(1, &VBO_grid_cell_mats_);

	glBindVertexArray(VAO_grid_cell_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_cell_instance_);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(glm::vec3), &k_cube_verts[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_cell_float_val_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_CELLS * sizeof(float), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_cell_mats_);
	glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_GRID_CELLS * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
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
	debug_instance_shader_("debug/simple_instance.vert", "debug/simple_instance.frag"),
	debug_grid_cell_shader_("debug/cell_visualization.vert", "debug/cell_visualization.frag"),
	debug_particle_shader_("debug/particle.vert", "debug/particle.frag"),
	frame_time_display_("0.0 ms/frame")
{
	SetVariableDefaults();
	SetupOriginBuffers();
	SetupGridLineBuffers();
	SetupGridVelocityBuffers();
	SetupGridAxisVelocityBuffers();
	SetupGridCellBuffers();

	pic_flip_renderer_ = new WaterParticleRenderer();

	ToggleDebugView(GRID_VELOCITIES);
	ToggleDebugView(FRAME_TIME);
}

DebugRenderer::~DebugRenderer()
{
	glDeleteBuffers(1, &VAO_grid_lines_);
	glDeleteBuffers(1, &VBO_line_instance_);
	glDeleteBuffers(1, &VBO_line_mats_);
	glDeleteBuffers(1, &VBO_line_color_);

	glDeleteBuffers(1, &VAO_grid_arrows_);
	glDeleteBuffers(1, &VBO_grid_arrow_instance_);
	glDeleteBuffers(1, &VBO_grid_arrow_mats_);
	glDeleteBuffers(1, &VBO_grid_arrow_colors_);

	glDeleteBuffers(1, &VAO_grid_axis_arrows_);
	glDeleteBuffers(1, &VBO_grid_axis_arrow_instance_);
	glDeleteBuffers(1, &VBO_grid_axis_arrow_mats_);
	glDeleteBuffers(1, &VBO_grid_axis_arrow_colors_);

	delete pic_flip_renderer_;
}

void DebugRenderer::SetGridBoundaries(const glm::vec3& low_bound, const glm::vec3& high_bound, const float interval)
{
	ws_grid_lower_bound_ = low_bound;
	ws_grid_upper_bound_ = high_bound;
	ws_grid_cell_size_ = interval;
	UpdateGridLines();
	UpdateGridCells((high_bound.x - low_bound.x) / interval);
}

void DebugRenderer::SetGridVelocities(const std::vector<glm::vec3>& grid_velocities, const unsigned int grid_dimensions)
{
	UpdateGridVelocities(grid_velocities, grid_dimensions);
	UpdateGridAxisVelocities(grid_velocities, grid_dimensions);
}

void DebugRenderer::SetGridPressures(const std::vector<float>& grid_pressures, const unsigned int grid_dim)
{
	if (active_cell_view_ != PRESSURE) {
		active_cell_view_ = PRESSURE;
		debug_grid_cell_shader_.SetUniform3fv("empty_color", glm::vec3(0.2, 0.2, 0.9));
		debug_grid_cell_shader_.SetUniform3fv("full_color", glm::vec3(0.9, 0.25, 0.1));
	}
	UpdateGridCellFloats(grid_pressures, grid_dim);
}

void DebugRenderer::SetGridDyeDensities(const std::vector<float>& grid_dyes, const unsigned int grid_dim)
{
	if (active_cell_view_ != DYE) {
		active_cell_view_ = DYE;
		debug_grid_cell_shader_.SetUniform3fv("empty_color", glm::vec3(0.0, 0.2, 0.9));
		debug_grid_cell_shader_.SetUniform3fv("full_color", glm::vec3(0.0, 0.25, 0.1));
	}
	UpdateGridCellFloats(grid_dyes, grid_dim);
}

void DebugRenderer::SetGridFluidCells(const std::vector<float>& grid_fluid, const unsigned int grid_dim)
{
	if (active_cell_view_ != IS_FLUID) {
		active_cell_view_ = IS_FLUID;
		debug_grid_cell_shader_.SetUniform3fv("empty_color", glm::vec3(0.4, 0.4, 0.4));
		debug_grid_cell_shader_.SetUniform3fv("full_color", glm::vec3(0.3, 0.3, 0.9));
	}
	UpdateGridCellFloats(grid_fluid, grid_dim);
}


bool DebugRenderer::SetParticlePositions(const Texture2D& particle_positions)
{
	// TODO: handle the const
	Texture2D particle_positions_copy = particle_positions;
	pic_flip_renderer_->UpdateParticlePositionsTexture(particle_positions_copy);
	return true;
}

bool DebugRenderer::SetParticleVelocities(const Texture2D& particle_velocities)
{
	// TODO: update particle velocity arrows
	return false;
}

bool DebugRenderer::SetView(const glm::mat4& view)
{
	cached_view_ = view;
	// Update the uniforms for shaders
	debug_instance_shader_.SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	debug_grid_cell_shader_.SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	return true;
}

bool DebugRenderer::SetProjection(const glm::mat4& proj)
{
	cached_proj_ = proj;
	// Update the uniforms for shaders
	debug_instance_shader_.SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	debug_grid_cell_shader_.SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
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

bool DebugRenderer::IsDebugViewActive(DebugView view)
{
	return active_views_.count(view) != 0;
}

void DebugRenderer::SetDebugCellView(GridCellView view)
{
	active_cell_view_ = view;
}

bool DebugRenderer::IsCellViewActive(GridCellView view)
{
	return active_cell_view_ == view;
}

DebugRenderer::GridCellView DebugRenderer::GetCellViewActive()
{
	return active_cell_view_;
}

bool DebugRenderer::Draw(Camera& camera, Skybox& skybox)
{
	for (auto& view : active_views_) {
		switch (view) {
		case ORIGIN:
			debug_instance_shader_.SetActive();
			glBindVertexArray(VAO_origin_lines_);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 3);
			glBindVertexArray(0);
			break;
		case GRID:
			// Draw Grid Lines
			debug_instance_shader_.SetActive();
			glBindVertexArray(VAO_grid_lines_);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 36, grid_line_elements_);
			glBindVertexArray(0);
			break;
		case GRID_AXIS_VELOCITIES:
			debug_instance_shader_.SetActive();
			glBindVertexArray(VAO_grid_axis_arrows_);
			glDrawArraysInstanced(GL_TRIANGLES, 0, grid_arrow_instance_num_, grid_axis_arrow_elements_);
			glBindVertexArray(0);
			break;
		case GRID_VELOCITIES:
			debug_instance_shader_.SetActive();
			glBindVertexArray(VAO_grid_arrows_);
			glDrawArraysInstanced(GL_TRIANGLES, 0, grid_arrow_instance_num_, grid_arrow_elements_);
			glBindVertexArray(0);
			break;
		case GRID_CELL:
			debug_grid_cell_shader_.SetActive();
			glBindVertexArray(VAO_grid_cell_);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 36, grid_cell_elements_);
			glBindVertexArray(0);
			break;
		case PARTICLES:
			// TODO
			pic_flip_renderer_->Draw(camera, skybox);
			break;
		case PARTICLE_VELOCITIES:
			// TODO
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
