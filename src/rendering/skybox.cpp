#include "skybox.hpp"

Skybox::Skybox(std::vector<std::string> input_textures)
	: Cubemap(input_textures), skybox_shader_(nullptr), VAO_(0)
{
	unsigned int VBO;

	glGenVertexArrays(1, &VAO_);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), &kSkyboxVertices_[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0); // Unbind this vertex array

	skybox_shader_ = new Shader("skybox.vert", "skybox.frag");
	skybox_shader_->SetUniformTexture2D("skybox", *this, GL_TEXTURE0);
}

bool Skybox::Draw()
{
	if (!valid_texture_) {
		return false;
	}
	glDepthFunc(GL_LEQUAL);
	skybox_shader_->SetActive();
	glBindVertexArray(VAO_);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
	return true;
}

bool Skybox::SetShader(Shader& s) {
	if (!valid_texture_) {
		return false;
	}
	skybox_shader_ = &s;
	return true;
}

bool Skybox::SetProjection(const glm::mat4& projection)
{
	if (!valid_texture_) {
		return false;
	}
	cached_proj_ = projection;
	skybox_shader_->SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	return true;
}

bool Skybox::SetView(const glm::mat4& view)
{
	if (!valid_texture_) {
		return false;
	}
	cached_view_ = glm::mat4(glm::mat3(view));
	skybox_shader_->SetUniformMatrix4fv("proj_view", cached_proj_ * cached_view_);
	return true;
}