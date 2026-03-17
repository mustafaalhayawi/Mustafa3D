#pragma once
#include "MathUtils.h"
#include "Window.h"

class Camera {
public:
	Camera(Vector3 pos, float pitch, float yaw, float FOV);

	template<typename T = ScreenPosition>
	T spaceToScreen(Vector3 worldPos, int screenWidth, int screenHeight) const;

	void update(MovementKeys movementKeys, float deltaTime);
	void updateOrientation();
	void processInput(MovementKeys movementKeys, float deltaTime);

	void strafeRight(float distance);
	void moveVertical(float distance);
	void moveForward(float distance);

	void setFov(float FOV);

	Vector3 position;

private:
	float m_moveSpeed = 5.0f; // speed of camera movement
	float m_rotateSpeed = 50.0f; // speed of camera rotation

	float m_pitch; // rotation about x-axis
	float m_yaw; // rotation about y-axis

	float m_fov;
	float m_projectionScale;

	float m_cosPitch, m_sinPitch;
	float m_cosYaw, m_sinYaw;
}; 

template<typename T>
T Camera::spaceToScreen(Vector3 worldPos, int screenWidth, int screenHeight) const {
	T screenPosition;

	float localX = worldPos.x - position.x;
	float localY = worldPos.y - position.y;
	float localZ = worldPos.z - position.z;

	float rotatedX = localX * m_cosYaw - localZ * m_sinYaw;
	float rotatedZ = localX * m_sinYaw + localZ * m_cosYaw;

	float pitchedY = localY * m_cosPitch - rotatedZ * m_sinPitch;
	float pitchedZ = localY * m_sinPitch + rotatedZ * m_cosPitch;

	if (pitchedZ < 0.1f) {
		screenPosition.z = -1.0f;
		return screenPosition;
	}

	float aspect = static_cast<float>(screenHeight) / screenWidth;

	float normalizedX = (rotatedX / pitchedZ) * m_projectionScale * aspect;
	float normalizedY = (pitchedY / pitchedZ) * m_projectionScale;

	float finalX = (normalizedX + 1.0f) * 0.5f * screenWidth;
	float finalY = (1.0f - normalizedY) * 0.5f * screenHeight;

	screenPosition.x = static_cast<decltype(screenPosition.x)>(finalX);
	screenPosition.y = static_cast<decltype(screenPosition.y)>(finalY);
	screenPosition.z = pitchedZ;

	return screenPosition;
}