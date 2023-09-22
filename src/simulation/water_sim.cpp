#include "water_sim.hpp"

/*
* Code taken and modified from 
* http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/
*/
PicFlipRenderer::PicFlipRenderer() 
	: particle_shader_("water_particle_shader.vert", "water_particle_shader.frag"),
	billboard_buffer_(0), position_buffer_(0)
{
	glGenBuffers(1, &billboard_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_buffer_);
	glBufferData(GL_ARRAY_BUFFER, kParticleQuadData_.size() * sizeof(glm::vec3), &kParticleQuadData_[0], GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	glGenBuffers(1, &position_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer_);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);

	// particle_shader_.SetUniform3fv("ws_light_dir", glm::normalize(glm::vec3(0.0, -0.2, -0.5)));
	particle_shader_.SetUniform1fv("particle_radius", 0.1f);
}

void PicFlipRenderer::UpdateParticlePositions(std::vector<glm::vec3>& positions)
{
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer_);
	// Buffer orphaning (improves streaming data performance)-> http://www.opengl.org/wiki/Buffer_Object_Streaming
	glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW); 
	glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec3), (void*) &positions[0]);

	particle_count_ = positions.size();
}

void PicFlipRenderer::Draw(Camera& cam)
{
	glm::mat4 view_mat = cam.GetViewMatrix();
	particle_shader_.SetUniform3fv("ws_camera_right", { view_mat[0][0], view_mat[1][0], view_mat[2][0] });
	printf("ws_camera_right: {%f, %f, %f}\n", view_mat[0][0], view_mat[1][0], view_mat[2][0]);
	particle_shader_.SetUniform3fv("ws_camera_up", { view_mat[0][1], view_mat[1][1], view_mat[2][1] });
	printf("ws_camera_up: {%f, %f, %f}\n", view_mat[0][1], view_mat[1][1], view_mat[2][1]);
	particle_shader_.SetUniformMatrix4fv("proj_view", cam.GetProjectionMatrix() * view_mat);

	particle_shader_.SetActive();

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_buffer_);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	// 2nd attribute buffer : positions of particles' centers
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer_);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0	);

	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particle_count_);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
