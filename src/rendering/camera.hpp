#ifndef CAMERA_C
#define CAMERA_C

#include <glm/glm.hpp>


class Camera {
private:
	/*
	* @brief
	* These are all the major fields that define a camera.
	* - Position is obvious
	* 
	* - Up is the direction the camera is tilted. 
	*	This is usually just straight up in Y in most cases.
	* 
	* - Lookat is the point that the camera is facing. It is essentially
	*	the point of focus.
	* 
	*/
	glm::vec3 position_;
	glm::vec3 up_;
	glm::vec3 look_at_;

	float aspect_ratio_;
	float fov_; // Stored in radians
	float near_plane_;
	float far_plane_;

public:
	/*
	* @brief
	* Constructs a camera object for ease of use. 
	* Use GetViewMatrix and GetProjectionMatrix for
	* getting the matrices to pass to shaders.
	* 
	* @param
	* position:
	* The position of the camera. This will affect
	* the view matrix, so if the position of the camera
	* changes with SetPosition, the view matrix will need to be updated.
	* 
	* @param
	* up:
	* The way the camera is oriented in terms of what way the camera top is facing towards.
	* If this is not directly pointing upwards, then the image will be tilted.
	* 
	* @param
	* look_at:
	* The point that the camera is looking at. 
	* This will most often be the focus of the scene.
	* 
	* @param
	* aspect_ratio:
	* The aspect_ratio given in width / height.
	* This defaults to 4:3. 
	* SetAspectRatio can be used to change this, and the
	* resulting projection matrix will change and thus need to be updated.
	* 
	* @param
	* fov:
	* The field of view of the camera, given in degrees. This determines how much the camera can
	* see at once. Defaults to 60 degrees.
	* 
	* @param
	* near_plane:
	* The clipping plane near the camera (i.e., the lower bound on what is within the camera's view). 
	* Given as some distance from the view direction.
	* Default is 0.1
	* 
	* @param
	* far_plane:
	* The clipping plane far from the camera (i.e., the upper bound on what is within the camera's view). 
	* Given as some distance from the view direction.
	* Default is 10.
	*/
	Camera(glm::vec3 position, glm::vec3 up, glm::vec3 look_at, float aspect_ratio = 4.0f/3.0f, float fov = 60.0f, float near_plane = 0.1f, float far_plane = 10.0f);
	~Camera();

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	glm::vec3 GetPosition();

	/*
	* Setters that affect the view matrix
	*/
	void SetPosition(glm::vec3 position);
	void SetUp(glm::vec3 up);
	void SetLookat(glm::vec3 look_at);

	/*
	* Setters that affect the projection matrix
	*/
	void SetAspectRatio(double width, double height);
	void SetFieldOfView(float degrees);
	void SetNearPlane(float z);
	void SetFarPlane(float z);

};

#endif