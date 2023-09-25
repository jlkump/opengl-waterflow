#include "water_sim.hpp"
/*
* Code taken and modified from 
* http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/
*/

////////////////////
// Initialization //
////////////////////

void PicFlipRenderer::InitializeParticleRenderingVariables()
{
	glGenVertexArrays(1, &particle_VAO_);
	glBindVertexArray(particle_VAO_);

	glGenBuffers(1, &particle_billboard_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, particle_billboard_buffer_);
	glBufferData(GL_ARRAY_BUFFER, kQuadData_.size() * sizeof(glm::vec3), &kQuadData_[0], GL_STATIC_DRAW);

	// The VBO containing the positions of particles
	glGenBuffers(1, &particle_position_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, particle_position_buffer_);
	glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);

	// render to texture setup
	glGenFramebuffers(1, &particle_frame_buffer_id_);
	glBindFramebuffer(GL_FRAMEBUFFER, particle_frame_buffer_id_);

	// Note: Add glGenRenderbuffers for depth buffer if there is wierdness with depth
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, viewport_width_, viewport_height_);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, depth_texture_.GetTextureId(), 0);

	GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		fprintf(stderr, "Problem with framebuffer\n");

	// We set particle radius here. TODO: Parametrizable radius

	// Cleanup
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PicFlipRenderer::InitializeScreenQuadVariables()
{
	glGenVertexArrays(1, &quad_VAO_);
	glBindVertexArray(quad_VAO_);

	glGenBuffers(1, &quad_position_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, quad_position_buffer_);
	glBufferData(GL_ARRAY_BUFFER, kQuadData_.size() * sizeof(glm::vec3), &kQuadData_[0], GL_STATIC_DRAW);

	glGenBuffers(1, &quad_index_buffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_index_buffer_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, kQuadIndices_.size() * sizeof(unsigned short), &kQuadIndices_[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

PicFlipRenderer::PicFlipRenderer()
	: particle_shader_("water_particle_shader.vert", "water_particle_shader.frag"),
	particle_billboard_buffer_(0), particle_position_buffer_(0), particle_VAO_(0), depth_texture_(glm::ivec2(viewport_width_, viewport_height_)),
	quad_VAO_(0), quad_position_buffer_(0),
	smoothing_shader_("screen_quad.vert", "water_smooth_depth.frag"),
	smoothed_depth_texture_(glm::ivec2(viewport_width_, viewport_height_)), // TODO, have callback to change based on viewport size
	water_shader_("screen_quad.vert", "water_shader.frag")
{
	InitializeParticleRenderingVariables();
	InitializeScreenQuadVariables();

	// Particle shader uniform
	particle_shader_.SetUniform1fv("particle_radius", 0.03f);

	// Smoothing shader uniforms
	smoothing_shader_.SetUniform1fv("blur_depth_falloff", 1.0);
	smoothing_shader_.SetUniform1fv("filter_radius", 0.1);
	smoothing_shader_.SetUniform1fv("blur_scale", 1.1);
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

/////////////
// Drawing //
/////////////

void PicFlipRenderer::DrawParticleSprites(glm::mat4& view_mat, glm::mat4& proj_mat)
{
	particle_shader_.SetUniform3fv("ws_camera_right", { view_mat[0][0], view_mat[1][0], view_mat[2][0] });
	particle_shader_.SetUniform3fv("ws_camera_up", { view_mat[0][1], view_mat[1][1], view_mat[2][1] });
	particle_shader_.SetUniformMatrix4fv("view", view_mat);
	particle_shader_.SetUniformMatrix4fv("proj", proj_mat);
	particle_shader_.SetUniformMatrix4fv("proj_view", proj_mat * view_mat);


	// set our depth_texture_ as the frame buffer
	particle_shader_.SetActive();
	glBindFramebuffer(GL_FRAMEBUFFER, particle_frame_buffer_id_);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, viewport_width_, viewport_height_);
	
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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PicFlipRenderer::Draw(Camera& cam)
{
	glm::mat4 view_mat = cam.GetViewMatrix();
	glm::mat4 proj_mat = cam.GetProjectionMatrix();
	DrawParticleSprites(view_mat, proj_mat);

	// Smooth the depth
	smoothing_shader_.SetActive();
	smoothing_shader_.SetUniformTexture("depth_sampler", depth_texture_, GL_TEXTURE0);

	glBindVertexArray(quad_VAO_);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_position_buffer_);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_index_buffer_);
	glDrawElements(GL_TRIANGLES, kQuadIndices_.size(), GL_UNSIGNED_SHORT, 0);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}
