#pragma once

#ifndef _SIMVISUALIZER_
#define _SIMVISUALIZER_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Camera.h"

class SimVisualizer
{
public:

	SimVisualizer(size_t num_particles, bool shadows = true,
		size_t width = 800, size_t heigth = 600);

	~SimVisualizer();

	void SetClearColor(glm::vec3 rgb);

	bool ErrorHappened();

private:

	bool ERROR = false;
	// window created
	GLFWwindow* m_window = nullptr;

	const size_t m_num_p;

	bool m_shadowsEnabled;

	Camera m_camera = Camera(glm::vec3(0.5f, 0.5f, 5.0f));
	glm::vec3 m_lightPosition = glm::vec3(0.5f, 1.0f, 3.0f);
	glm::vec3 m_lightColor = glm::vec3(0.6f, 0.6f, 0.6f);
	glm::vec3 m_ambientLight = glm::vec3(0.2f, 0.2f, 0.2f);

	bool m_firstMouse = true;
	float m_lastX, m_lastY;

	size_t m_SCR_WIDTH, m_SCR_HEIGHT;

	GLuint m_VAO_particles, m_VBO_particles[2];
	GLuint m_m_VAO_BB, m_VBO_BB[2];
	GLuint m_depthFBO;
	GLuint m_depthMapTex;


	bool initGLFW();
	bool initOpenGL();

	void setMouseInteractive(bool interactive);

	void setCallbacks();
	// CALLBACKS
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

};

#endif // !_SIMVISUALIZER_
