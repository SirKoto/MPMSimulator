#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>






class Camera
{
public:

	enum class Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_worldUP;

	float m_yaw, m_pitch;
	
	float m_speed, m_mouseSens, m_zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = -90.0f, float pitch = 0.0f) :
		m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
		m_speed(2.5f), m_mouseSens(0.1f), m_zoom(45.0f)
	{
		m_position = position;
		m_worldUP = up;
		m_yaw = yaw;
		m_pitch = pitch;

		updateCameraVectors();
	}

	~Camera() = default;

	const glm::mat4 GetViewMatrix() const
	{
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}

	void ProcessKeyboard(Camera_Movement dir, float dt)
	{
		float v = m_speed * dt;
		switch (dir)
		{
		case Camera_Movement::FORWARD:
			m_position += m_front * v;
			break;
		case Camera_Movement::BACKWARD:
			m_position -= m_front * v;
			break;
		case Camera_Movement::LEFT:
			m_position += m_right * v;
			break;
		case Camera_Movement::RIGHT:
			m_position -= m_right * v;
			break;
		default:
			assert(!"YOU SHOULD NOT BE HERE");
			break;
		}
	}

	void ProcessMouseMovement(float xoffset, float yoffset)
	{
		xoffset *= m_mouseSens;
		yoffset *= m_mouseSens;

		m_yaw += xoffset;
		m_pitch -= yoffset;

		if (m_pitch > 89.0f)
			m_pitch = 89.0f;
		if (m_pitch < -89.0f)
			m_pitch = -89.0f;

		updateCameraVectors();
	}

	void ProcessMouseScroll(float yoffset)
	{
		if (m_zoom >= 1.0f && m_zoom < 45.0f)
			m_zoom -= yoffset;
		if (m_zoom <= 1.0f)
			m_zoom = 1.0f;
		if (m_zoom >= 45.0f)
			m_zoom = 45.0f;
	}

private:

	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		front.y = sin(glm::radians(m_pitch));
		front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_front = glm::normalize(front);

		m_right = glm::normalize(glm::cross(m_front, m_worldUP));
		m_up = glm::normalize(glm::cross(m_right, m_front));
	}
};

#endif //CAMERA_H

