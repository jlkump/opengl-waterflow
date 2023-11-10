#include "fps_camera.hpp"

FPSCamera::FPSCamera(Camera* cam) :
	is_forward_pressed_(false),
	is_back_pressed_(false),
	is_left_pressed_(false),
	is_right_pressed_(false),
	is_up_pressed_(false),
	is_down_pressed_(false),
	move_speed_(0.01f),
	cam_(cam)
{
}

FPSCamera::FPSCamera(glm::vec3 start_pos, glm::vec3 look_pos, glm::vec3 up) : 
	is_forward_pressed_(false),
	is_back_pressed_(false),
	is_left_pressed_(false),
	is_right_pressed_(false),
	is_up_pressed_(false),
	is_down_pressed_(false),
	move_speed_(0.2f),
	cam_(nullptr)
{
	cam_ = new Camera(start_pos, look_pos, up);
}

void FPSCamera::Process(float delta)
{
	if (cam_ == nullptr) {
		return;
	}

	glm::vec3 input_vector = glm::vec3(0, 0, 0);
	if (is_forward_pressed_) {
		input_vector += glm::vec3(0.0, 0.0, 1.0);
	}
	if (is_back_pressed_) {
		input_vector += glm::vec3(0.0, 0.0, -1.0);
	}
	if (is_left_pressed_) {
		input_vector += glm::vec3(-1.0, 0.0, 0.0);
	}
	if (is_right_pressed_) {
		input_vector += glm::vec3(1.0, 0.0, 0.0);
	}
	if (is_up_pressed_) {
		input_vector += glm::vec3(0.0, 1.0, 0.0);
	}
	if (is_down_pressed_) {
		input_vector += glm::vec3(0.0, -1.0, 0.0);
	}

	glm::vec3 new_pos = cam_->GetPosition() 
		+ cam_->GetForward() * move_speed_ * input_vector.z 
		+ cam_->GetRight() * move_speed_ * input_vector.x 
		+ cam_->GetUp() * move_speed_ * input_vector.y;
	cam_->SetPosition(new_pos);
}

void FPSCamera::UpPressed()
{
	printf("Up pressed\n");
	is_up_pressed_ = true;
}

void FPSCamera::UpReleased()
{
	printf("Up released\n");
	is_up_pressed_ = false;
}

void FPSCamera::DownPressed()
{
	printf("Down pressed\n");
	is_down_pressed_ = true;
}

void FPSCamera::DownReleased()
{
	printf("Down released\n");
	is_down_pressed_ = false;
}

void FPSCamera::ForwardPressed()
{
	printf("Forward pressed\n");
	is_forward_pressed_ = true;
}

void FPSCamera::ForwardReleased()
{
	printf("Forward released\n");
	is_forward_pressed_ = false;
}

void FPSCamera::BackPressed()
{
	printf("Back pressed\n");
	is_back_pressed_ = true;
}

void FPSCamera::BackReleased()
{
	printf("Back released\n");
	is_back_pressed_ = false;
}

void FPSCamera::LeftPressed()
{
	printf("left pressed\n");
	is_left_pressed_ = true;
}

void FPSCamera::LeftReleased()
{
	printf("left released\n");
	is_left_pressed_ = false;
}

void FPSCamera::RightPressed()
{
	printf("right pressed\n");
	is_right_pressed_ = true;
}

void FPSCamera::RightReleased()
{
	printf("right released\n");
	is_right_pressed_ = false;
}

void FPSCamera::SetMovespeed(float speed)
{
	move_speed_ = speed;
}

void FPSCamera::SetAspectRatio(double width, double height)
{
	cam_->SetAspectRatio(width, height);
}

void FPSCamera::SetFOV(double degrees)
{
	cam_->SetFieldOfView(degrees);
}

Camera* FPSCamera::GetCam()
{
	return cam_;
}
