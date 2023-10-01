#ifndef CUBEMAP_C
#define CUBEMAP_C


#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"

/*
* @brief
* A class that is fairly similar to the texture class,
* but instead manages the boilerplate for creating cubemaps.
* 
*/
class Cubemap : public Texture {
public:
	/*
	* @brief
	* Loads in images given in the vector of string paths
	* for each face of the cubemap. The given files are
	* assumed to be in the order of:
	*		- "right" or positive X
	*		- "left" or negative X
	*		- "top" or positive Y
	*		- "bottom" or negative Y
	*		- "front" or positive Z
	*		- "back" or negative Z
    * 
    * @param
    * skybox:
    * An optional parameter to set whether this cubemap is a skybox
    * or not. Default is true
	*/
	Cubemap(std::vector<std::string> input_textures);
	~Cubemap();

    virtual void ActiveBind(GLenum texture_unit);
};

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
    Shader skybox_shader_;

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
    void Draw(glm::mat4 view, glm::mat4 projection);
    void SetShader(Shader& s);
};


#endif // !CUBEMAP_C