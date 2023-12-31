#ifndef FPS_CAMERA_H
#define FPS_CAMERA_H

#include "camera.hpp"

#include <unordered_set>

class FPSCamera {
private:
	enum Action {
		MOVE_FORWARD,
		MOVE_BACK,
		MOVE_LEFT,
		MOVE_RIGHT,
		MOVE_UP,
		MOVE_DOWN,
		ROTATE_CW,
		ROTATE_CCW,
		LOOK_DOWN,
		LOOK_UP,
	};

	std::unordered_set<Action> active_actions_;

	float move_speed_;
	float rotate_speed_;
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
	void RotateCwPressed();
	void RotateCwReleased();
	void RotateCCwPressed();
	void RotateCCwReleased();

	void SetMovespeed(float speed);

	void SetAspectRatio(double width, double height);
	void SetFOV(double degrees);

	Camera* GetCam();
};

#endif // !FPS_CAMERA_H
