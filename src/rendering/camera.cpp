#include "camera.hpp"
#include "camera.hpp"
#include "camera.hpp"
#include "camera.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(glm::vec3 position, glm::vec3 look_at, glm::vec3 up, float aspect_ratio, float fov, float near_plane, float far_plane)
	: position_(position), look_at_(look_at), up_(up), aspect_ratio_(aspect_ratio), fov_(glm::radians(fov)), near_plane_(near_plane), far_plane_(far_plane)
{
	up_ = glm::normalize(up_);
}

Camera::~Camera()
{
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(position_, look_at_, up_);
}

glm::mat4 Camera::GetProjectionMatrix()
{
	return glm::perspective(fov_, aspect_ratio_, near_plane_, far_plane_);
}

glm::vec3 Camera::GetPosition()
{
	return position_;
}

glm::vec3 Camera::GetForward()
{
	return glm::normalize(look_at_ - position_);
}

glm::vec3 Camera::GetUp()
{
	return up_;
}

glm::vec3 Camera::GetRight()
{
	return glm::normalize(glm::cross(GetForward(), GetUp()));
}



void Camera::SetPosition(glm::vec3 position)
{
	position_ = position;
}

void Camera::SetUp(glm::vec3 up)
{
	up_ = up;
}

void Camera::SetLookat(glm::vec3 look_at)
{
	look_at_ = look_at;
}

void Camera::SetAspectRatio(double width, double height)
{
	aspect_ratio_ = width / height;
}

void Camera::SetFieldOfView(float degrees)
{
	fov_ = glm::radians(degrees);
}

void Camera::SetNearPlane(float z)
{
	near_plane_ = z;
}

void Camera::SetFarPlane(float z)
{
	far_plane_ = z;
}
