#include "display_text.hpp"
#include "texture.hpp"
#include "shader.hpp"

#include <unordered_map>

static Texture2D* text_texture_singleton;
static Shader* text_shader_singleton;

DisplayText::DisplayText(const std::string& text)
	: text_(text), character_scale_(0.1), text_pos_(glm::vec2(-1.0, -0.8)),
	VAO_(0), VBO_(0), EBO_(0), draw_amount_(0)
{
	if (text_texture_singleton == nullptr) {
		text_texture_singleton = new Texture2D("text.png");
	}
	if (text_shader_singleton == nullptr) {
		text_shader_singleton = new Shader("debug/text_shader.vert", "debug/text_shader.frag");
		text_shader_singleton->SetUniformTexture2D("textureSampler", *text_texture_singleton, GL_TEXTURE0);
	}
	glGenVertexArrays(1, &VAO_);
	glGenBuffers(1, &VBO_);
	glGenBuffers(1, &EBO_);
	SetText(text);
}

DisplayText::~DisplayText()
{
	glDeleteBuffers(1, &VAO_);
	glDeleteBuffers(1, &VBO_);
	glDeleteBuffers(1, &EBO_);
}

bool DisplayText::SetText(const std::string& text)
{
	text_ = text;

	std::vector<TextVert> verts;
	std::vector<unsigned char> indices;

	for (int i = 0; i < text_.size(); i++) {
		char current_char = text_[i];
		// printf("Putting char %c in texture\n", current_char);
		float uv_x = (current_char % 16) / 16.0f;
		float uv_y = (current_char / 16) / 16.0f;

		glm::vec2 v_upper_left = text_pos_ + glm::vec2(i * character_scale_ / 2, 0);
		glm::vec2 v_lower_left = text_pos_ + glm::vec2(i * character_scale_ / 2, -character_scale_);
		glm::vec2 v_upper_right = text_pos_ + glm::vec2((i + 1) * character_scale_ / 2, 0);
		glm::vec2 v_lower_right = text_pos_ + glm::vec2((i + 1) * character_scale_ / 2, -character_scale_);

		/*
		printf("Char has vertices:\n"
			"%4.4f %4.4f\n"
			"%4.4f %4.4f\n"
			"%4.4f %4.4f\n"
			"%4.4f %4.4f\n", 
			v_upper_left.x, v_upper_left.y,
			v_lower_left.x, v_lower_left.y,
			v_upper_right.x, v_upper_right.y,
			v_lower_right.x, v_lower_right.y);
		*/

		glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
		glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
		glm::vec2 uv_low_right = glm::vec2(uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
		glm::vec2 uv_low_left = glm::vec2(uv_x, (uv_y + 1.0f / 16.0f));

		verts.push_back(TextVert(v_upper_left, uv_up_left));
		verts.push_back(TextVert(v_lower_left, uv_low_left));
		verts.push_back(TextVert(v_upper_right, uv_up_right));
		verts.push_back(TextVert(v_lower_right, uv_low_right));

		indices.push_back(i * 4);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 3);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 1);

		/*
		printf("Char has indices:\n"
			"%d, %d, %d, %d, %d, %d\n",
			i * 4, i * 4 + 1, i * 4 + 2, i * 4 + 3, i * 4 + 2, i * 4 + 1);
		*/
	}

	glBindVertexArray(VAO_);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO_);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(TextVert), &verts[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned char), &indices[0], GL_DYNAMIC_DRAW);
	draw_amount_ = indices.size();
	// vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVert), (void*)0);
	// vertex texture coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVert), (void*)offsetof(TextVert, uv_));

	glBindVertexArray(0);

	return true;
}

bool DisplayText::SetPosition(const glm::vec2& hs_position)
{
	text_pos_ = hs_position;
	return SetText(text_);
}

bool DisplayText::SetScale(const float scale)
{
	character_scale_ = scale;
	return SetText(text_);
}

bool DisplayText::Draw()
{
	// Bind shader
	text_shader_singleton->SetActive();

	// Bind texture
	text_texture_singleton->ActiveBind(GL_TEXTURE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(VAO_);
	glDrawElements(GL_TRIANGLES, draw_amount_, GL_UNSIGNED_BYTE, 0);
	glBindVertexArray(0);
	glDisable(GL_BLEND);
	return true;
}



