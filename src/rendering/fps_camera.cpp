#include "fps_camera.hpp"

FPSCamera::FPSCamera(Camera* cam) :
	move_speed_(0.002f),
	rotate_speed_(0.001f),
	cam_(cam)
{
}

FPSCamera::FPSCamera(glm::vec3 start_pos, glm::vec3 look_pos, glm::vec3 up) : 
	move_speed_(0.002f),
	rotate_speed_(0.001f),
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
	float rotate_angle = 0.0f;
	for (Action a : active_actions_) {
		switch (a) {
		case MOVE_FORWARD:
			input_vector += glm::vec3(0.0, 0.0, 1.0);
			break;
		case MOVE_BACK:
			input_vector += glm::vec3(0.0, 0.0, -1.0);
			break;
		case MOVE_UP:
			input_vector += glm::vec3(0.0, 1.0, 0.0);
			break;
		case MOVE_DOWN:
			input_vector += glm::vec3(0.0, -1.0, 0.0);
			break;
		case MOVE_LEFT:
			input_vector += glm::vec3(-1.0, 0.0, 0.0);
			break;
		case MOVE_RIGHT:
			input_vector += glm::vec3(1.0, 0.0, 0.0);
			break;
		case ROTATE_CW:
			rotate_angle += rotate_angle;
			break;
		case ROTATE_CCW:
			rotate_angle -= rotate_angle;
			break;
		}
	}
	glm::vec3 new_pos = cam_->GetPosition() 
		+ cam_->GetForward() * move_speed_ * input_vector.z 
		+ cam_->GetRight() * move_speed_ * input_vector.x 
		+ cam_->GetUp() * move_speed_ * input_vector.y;
	glm::vec3 new_lookat = cam_->GetForward() + new_pos;
	cam_->SetPosition(new_pos);
	cam_->SetLookat(new_lookat);
}

void FPSCamera::UpPressed()
{
	active_actions_.insert(MOVE_UP);
}

void FPSCamera::UpReleased()
{
	active_actions_.erase(MOVE_UP);
}

void FPSCamera::DownPressed()
{
	active_actions_.insert(MOVE_DOWN);
}

void FPSCamera::DownReleased()
{
	active_actions_.erase(MOVE_DOWN);
}

void FPSCamera::ForwardPressed()
{
	active_actions_.insert(MOVE_FORWARD);
}

void FPSCamera::ForwardReleased()
{
	active_actions_.erase(MOVE_FORWARD);
}

void FPSCamera::BackPressed()
{
	active_actions_.insert(MOVE_BACK);
}

void FPSCamera::BackReleased()
{
	active_actions_.erase(MOVE_BACK);
}

void FPSCamera::LeftPressed()
{
	active_actions_.insert(MOVE_LEFT);
}

void FPSCamera::LeftReleased()
{
	active_actions_.erase(MOVE_LEFT);
}

void FPSCamera::RightPressed()
{
	active_actions_.insert(MOVE_RIGHT);
}

void FPSCamera::RightReleased()
{
	active_actions_.erase(MOVE_RIGHT);
}

void FPSCamera::RotateCwPressed()
{
	active_actions_.insert(ROTATE_CW);
}

void FPSCamera::RotateCwReleased()
{
	active_actions_.erase(ROTATE_CW);
}

void FPSCamera::RotateCCwPressed()
{
	active_actions_.insert(ROTATE_CCW);
}

void FPSCamera::RotateCCwReleased()
{
	active_actions_.erase(ROTATE_CCW);
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
