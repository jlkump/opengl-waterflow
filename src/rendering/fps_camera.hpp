#ifndef FPS_CAMERA_H
#define FPS_CAMERA_H

#include "camera.hpp"

#include <unordered_set>

class FPSCamera {
private:
	bool is_forward_pressed_;
	bool is_back_pressed_;
	bool is_left_pressed_;
	bool is_right_pressed_;
	bool is_up_pressed_;
	bool is_down_pressed_;

	float move_speed_;

	Camera* cam_;

public:
	FPSCamera(Camera* cam);
	FPSCamera(glm::vec3 start_pos, glm::vec3 look_pos, glm::vec3 up = glm::vec3(0, 1, 0));

	void Process(float delta);
	void UpPressed();
	void UpReleased();
	void DownPressed();
	void DownReleased();

	void ForwardPressed();
	void ForwardReleased();
	void BackPressed();
	void BackReleased();
	void LeftPressed();
	void LeftReleased();
	void RightPressed();
	void RightReleased();

	void SetMovespeed(float speed);

	void SetAspectRatio(double width, double height);
	void SetFOV(double degrees);

	Camera* GetCam();
};

#endif // !FPS_CAMERA_H
