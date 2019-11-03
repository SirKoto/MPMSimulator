#pragma once

#ifndef _SIMVISUALIZER_
#define _SIMVISUALIZER_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Camera.h"
#include "IO/FrameSBF.h"

class SimVisualizer
{
public:

	SimVisualizer(size_t num_particles, bool shadows = true,
		size_t width = 800, size_t heigth = 600);

	~SimVisualizer();

	void SetClearColor(glm::vec3 rgb);

	void setShadowMapRes(size_t w, size_t h);

	void setScaleParticles(glm::vec3 scale);

	void reloadShaders();

	bool ErrorHappened() const;

	void updateParticlePositions(const float* pos);

	void updateParticlePositions(const FrameSBF<float>& frame);

	void updateParticlesColor(const float* color);

	void draw();

	bool& getEnterPressed() {
		return m_enterPressed;
	}

	bool shouldApplicationClose();

	void processKeyboardInput();

	void setDT(const float dt) {
		m_dt = dt;
	}
private:

	bool m_ERROR = false;
	// window created
	GLFWwindow* m_window = nullptr;

	const int m_num_p;

	float m_dt;

	bool m_shadowsEnabled;

	Shader m_shaders[3]; // [0] Particle, [1] BB, [2] ShadowMap
	Camera m_camera = Camera(glm::vec3(0.5f, 0.5f, 3.0f));
	
	glm::vec3 m_lightPosition = glm::vec3(0.5f, 1.0f, 3.0f);
	glm::vec3 m_lightColor = glm::vec3(0.6f, 0.6f, 0.6f);
	glm::vec3 m_ambientLight = glm::vec3(0.2f, 0.2f, 0.2f);

	glm::mat4 m_particleModel;
	glm::vec3 m_particleScale = glm::vec3(5e-3f);

	bool m_firstMouse = true;
	bool m_enterPressed = false;
	float m_lastX, m_lastY;

	int m_SCR_WIDTH, m_SCR_HEIGHT;
	int m_shadowTex_w = 2048, m_shadowTex_h = 2048;

	GLuint m_VAO_particles, m_VBO_particles[3];
	GLuint m_VAO_BB, m_VBO_BB;
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
	void drawParticles() const;
	void drawShadowMap() const;
};

#endif // !_SIMVISUALIZER_
