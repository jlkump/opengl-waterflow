#include "water_sim.hpp"
/*
* Code taken and modified from 
* http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/
*/

void PicFlipRenderer::InitializeParticleRenderingVariables()
{
	glGenVertexArrays(1, &particle_VAO_);
	glBindVertexArray(particle_VAO_);

	glGenBuffers(1, &particle_billboard_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, particle_billboard_buffer_);
	glBufferData(GL_ARRAY_BUFFER, kQuadData_.size() * sizeof(glm::vec3), &kQuadData_[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// The VBO containing the positions of particles
	glGenBuffers(1, &particle_position_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, particle_position_buffer_);
	glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// render to texture setup
	glGenFramebuffers(1, &frame_buffer_id_);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id_);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, depth_texture_.GetTextureId(), 0);
	GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_buffers);

	// We set particle radius here. TODO: Parametrizable radius
	particle_shader_.SetUniform1fv("particle_radius", 0.01f);
}

void PicFlipRenderer::InitializeScreenQuadVariables()
{
	glGenVertexArrays(1, &quad_VAO_);
	glBindVertexArray(quad_VAO_);

	glGenBuffers(1, &quad_position_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, quad_position_buffer_);
	glBufferData(GL_ARRAY_BUFFER, kQuadData_.size() * sizeof(glm::vec3), &kQuadData_[0], GL_STATIC_DRAW);

	glGenBuffers(1, &quad_index_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, quad_index_buffer_);
	glBufferData(GL_ARRAY_BUFFER, kQuadIndices_.size() * sizeof(unsigned int), &kQuadIndices_[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}


PicFlipRenderer::PicFlipRenderer()
	: particle_shader_("water_particle_shader.vert", "water_particle_shader.frag"),
	particle_billboard_buffer_(0), particle_position_buffer_(0), particle_VAO_(0), depth_texture_(1024),
	quad_VAO_(0), quad_position_buffer_(0),
	smoothing_shader_("screen_quad.vert", "smoothing.frag"),
	smoothed_depth_texture_(1024),
	water_shader_("screen_quad.vert", "water_shader.frag")
{
	InitializeParticleRenderingVariables();
	InitializeScreenQuadVariables();

}



void PicFlipRenderer::UpdateParticlePositions(std::vector<glm::vec3>& positions)
{
	glBindVertexArray(particle_VAO_);

	glBindBuffer(GL_ARRAY_BUFFER, particle_position_buffer_);
	// Buffer orphaning (improves streaming data performance)-> http://www.opengl.org/wiki/Buffer_Object_Streaming
	glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW); 
	glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec3), (void*) &positions[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	particle_count_ = positions.size();

	glBindVertexArray(0);
}

void PicFlipRenderer::DrawParticlesToTexture(glm::mat4& view_mat, glm::mat4& proj_mat)
{
	particle_shader_.SetUniform3fv("ws_camera_right", { view_mat[0][0], view_mat[1][0], view_mat[2][0] });
	particle_shader_.SetUniform3fv("ws_camera_up", { view_mat[0][1], view_mat[1][1], view_mat[2][1] });
	particle_shader_.SetUniformMatrix4fv("view", view_mat);
	particle_shader_.SetUniformMatrix4fv("proj", proj_mat);
	particle_shader_.SetUniformMatrix4fv("proj_view", proj_mat * view_mat);
	particle_shader_.SetUniform3fv("vs_light_dir", glm::inverse(view_mat) * glm::normalize(glm::vec4(0.0, 0.2, 0.5, 0.0)));


	particle_shader_.SetActive();
	depth_texture_.ActiveBind(GL_TEXTURE0);
	
	glBindVertexArray(particle_VAO_);
	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, particle_billboard_buffer_);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : positions of particles' centers
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, particle_position_buffer_);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Instance drawing
	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particle_count_);

	// Cleanup
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);

}

void PicFlipRenderer::Draw(Camera& cam)
{
	glm::mat4 view_mat = cam.GetViewMatrix();
	glm::mat4 proj_mat = cam.GetProjectionMatrix();
	DrawParticlesToTexture(view_mat, proj_mat);

	// Draw quad
	glBindVertexArray(quad_VAO_);
	glDrawElements(GL_TRIANGLES, kQuadIndices_.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
