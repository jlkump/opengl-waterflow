#ifndef DISPLAY_TEXT_H
#define DISPLAY_TEXT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

class Texture2D;
class Shader;

class DisplayText {
private:
	std::string text_;

	float character_scale_; // Characters are a square
	glm::vec2 text_pos_; // Left aligned assumed

	struct TextVert {
		glm::vec2 pos_;
		glm::vec2 uv_;
		TextVert(const glm::vec2& pos, const glm::vec2& uv) : pos_(pos), uv_(uv) {}
	};

	int draw_amount_;

	GLuint VAO_;
	GLuint VBO_;
	GLuint EBO_;

public:
	DisplayText(const std::string& text);
	~DisplayText();
	bool SetText(const std::string& text);
	bool SetPosition(const glm::vec2& hs_position);
	bool SetScale(const float scale);
	bool Draw();
};


#endif