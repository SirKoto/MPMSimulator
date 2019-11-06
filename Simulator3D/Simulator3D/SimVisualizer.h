#pragma once

#ifndef _SIMVISUALIZER_
#define _SIMVISUALIZER_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include <functional>

#include "Shader.h"
#include "Camera.h"
#include "IO/FrameSBF.h"

class SimVisualizer
{
public:

	enum class KEYS
	{
		U, I, O, P, K, L, ENTER
	};

	SimVisualizer(int num_particles, bool shadows = true,
		int width = 800, int heigth = 600);

	~SimVisualizer();

	void SetClearColor(glm::vec3 rgb);

	void setShadowMapRes(int w, int h);

	void setScaleParticles(glm::vec3 scale);

	void reloadShaders();

	bool ErrorHappened() const;

	void updateParticlePositions(const float* pos);

	void updateParticlePositions(const FrameSBF<float>& frame);

	void updateParticlesColor(const float* color);

	void draw();

	void enableUserInput(bool enable);

	bool shouldApplicationClose();

	void setKeyCallback(KEYS key, std::function<void()> f);

	void temptateEvents();
private:

	bool m_ERROR = false;
	// window created
	GLFWwindow* m_window = nullptr;

	const int m_num_p;

	float m_dt = 0;
	float m_t_last = 0;
	float m_time_press_t = 0;

	bool m_shadowsEnabled;

	bool m_userInputEnabled = true;

	Shader m_shaders[3]; // [0] Particle, [1] BB, [2] ShadowMap
	Camera m_camera = Camera(glm::vec3(0.5f, 0.5f, 3.0f));
	
	glm::vec3 m_lightPosition = glm::vec3(0.5f, 1.0f, 3.0f);
	glm::vec3 m_lightColor = glm::vec3(0.6f, 0.6f, 0.6f);
	glm::vec3 m_ambientLight = glm::vec3(0.2f, 0.2f, 0.2f);

	glm::mat4 m_particleModel;
	glm::vec3 m_particleScale = glm::vec3(5e-3f);

	bool m_firstMouse = true;
	float m_lastX, m_lastY;

	int m_SCR_WIDTH, m_SCR_HEIGHT;
	int m_shadowTex_w = 2048, m_shadowTex_h = 2048;

	GLuint m_VAO_particles, m_VBO_particles[3];
	GLuint m_VAO_BB, m_VBO_BB;
	GLuint m_depthFBO;
	GLuint m_depthMapTex;

	// personalizable callbacks U, I, O, P, K, L, ENTER
	std::function<void()> f_call[7];

	bool initGLFW();
	bool initOpenGL();

	void setMouseInteractive(bool interactive);

	void processKeyboardInput();

	void processKeyboardInputLess();

	void updateDT();

	void setCallbacks();

	void unsetCallbacks();

	// CALLBACKS
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void processKeyboardInputCallbacks();

	


	// ARRAYS
	void initArraysParticles();
	void initArraysBB();
	void initFBOShadows();

	// Shaders
	void updateUniforms();
	void updateModelMatrix();
	void setUniforms(Shader& s, const glm::mat4& projectionView);

	// Draw calls
	void drawBB() const;
	void drawParticles(bool enable = true) const;
	void drawShadowMap() const;
};

#endif // !_SIMVISUALIZER_
