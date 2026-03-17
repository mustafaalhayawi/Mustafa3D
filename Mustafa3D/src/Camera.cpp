#include "Camera.h"

Camera::Camera(Vector3 pos, float pitch, float yaw, float FOV)
	: position(pos),
	  m_pitch(pitch),
	  m_yaw(yaw),
	  m_fov(FOV)
{
	m_projectionScale = 1.0f / tan(FOV * (Math::pi / 180.0f) / 2);

	updateOrientation();
}

void Camera::update(MovementKeys movementKeys, float deltaTime) {
	processInput(movementKeys, deltaTime);
	updateOrientation();
}

void Camera::updateOrientation() {
	float pitchRad = m_pitch * Math::pi / 180.0f;
	float yawRad = m_yaw * Math::pi / 180.0f;

	m_cosPitch = cos(pitchRad);
	m_sinPitch = sin(pitchRad);
	m_cosYaw = cos(yawRad);
	m_sinYaw = sin(yawRad);
}

void Camera::processInput(MovementKeys movementKeys, float deltaTime) {
	float distance = deltaTime * m_moveSpeed;
	if (movementKeys.up && !movementKeys.down) moveVertical(distance);
	else if (movementKeys.down && !movementKeys.up) moveVertical(-distance);
	if (movementKeys.left && !movementKeys.right) strafeRight(-distance);
	else if (movementKeys.right && !movementKeys.left) strafeRight(distance);
	if (movementKeys.forward && !movementKeys.backward) moveForward(distance);
	else if (movementKeys.backward && !movementKeys.forward) moveForward(-distance);

	if (movementKeys.rotateLeft && !movementKeys.rotateRight) m_yaw -= deltaTime * m_rotateSpeed;
	if (movementKeys.rotateRight && !movementKeys.rotateLeft) m_yaw += deltaTime * m_rotateSpeed;
	if (movementKeys.rotateUp && !movementKeys.rotateDown) m_pitch += deltaTime * m_rotateSpeed;
	if (movementKeys.rotateDown && !movementKeys.rotateUp) m_pitch -= deltaTime * m_rotateSpeed;

	if (m_pitch < -90.0f) m_pitch = -90.0f;
	else if (m_pitch > 90.0f) m_pitch = 90.0f;
}

void Camera::strafeRight(float distance) {
	position.x += m_cosYaw * distance;
	position.z -= m_sinYaw * distance;
}

void Camera::moveVertical(float distance) {
	position.y += distance;
}

void Camera::moveForward(float distance) {
	position.x += m_sinYaw * distance;
	position.z += m_cosYaw * distance;
}

void Camera::setFov(float FOV) {
	if (0.0f < FOV && FOV < 180.0f) {
		m_fov = FOV;
		m_projectionScale = 1.0f / tan(FOV * (Math::pi / 180.0f) / 2);
	}
}