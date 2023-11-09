#ifndef SKYBOX_H
#define SKYBOX_H

#include "texture.hpp"
#include "shader.hpp"

/*
* @brief
* A sub-set of the Cubemap, a skybox is used for creating scenery
* which stays at a contant distance in the background of the scene.
* It is also very useful for making reflections.
*/
class Skybox : public Cubemap {
private:

	/*
	* @brief
	* A Skybox exists in the world, so we need the positions of the unit cube
	* We will move this cube with the camera given the view and projection
	* matrices in calls to Draw();
	*/
	GLuint VAO_;
	Shader* skybox_shader_;
	glm::mat4 cached_view_;
	glm::mat4 cached_proj_;

	// Taken from https://learnopengl.com/code_viewer.php?code=advanced/cubemaps_skybox_data
	const float kSkyboxVertices_[108] = {
		// positions       
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

public:
	Skybox(std::vector<std::string> input_textures);
	bool Draw();
	bool SetShader(Shader& s);
	/*
	* @brief
	* Update projection or view matrix used for the skybox.
	* Should only be called when the actual projection or view
	* matrix changes (and to initialize the projection and view matrix).
	*/
	bool SetProjection(const glm::mat4& projection);
	bool SetView(const glm::mat4& view);
};

#endif