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

	// Configurable Keys
	enum class KEYS
	{
		U, I, O, P, K, L, ENTER
	};

	// Constructor
	SimVisualizer(int num_particles, bool shadows = true,
		int width = 800, int heigth = 600);

	// Destructor
	~SimVisualizer();

	// Set background color
	void SetClearColor(glm::vec3 rgb);

	// Change shadow map resolution
	void setShadowMapRes(int w, int h);

	// Change size of the icospheres (particles)
	void setScaleParticles(glm::vec3 scale);

	// Reload shaders
	void reloadShaders();

	// Returns if the screen should close by ERROR
	bool ErrorHappened() const;

	// Change the particle positions on screen (not draw)
	void updateParticlePositions(const float* pos);

	// Change the particle positions on screen (not draw) according to frame
	void updateParticlePositions(const FrameSBF<float>& frame);

	// Change the vector of colors of the particles
	void updateParticlesColor(const float* color);

	// Draw on buffer and swap Color buffers
	void draw();

	// Enable and dissable complex user imput
	void enableUserInput(bool enable);

	// The aplication should close by user
	bool shouldApplicationClose();

	// Add or reconfigure some callback of a key
	void setKeyCallback(KEYS key, std::function<void()> f);

	// Identify and execute ONLY events
	void temptateEvents();



	inline const int& GetWidth() const { return m_SCR_WIDTH; }
	inline const int& GetHeight() const { return m_SCR_HEIGHT; }

private:

	bool m_ERROR = false;
	// window created
	GLFWwindow* m_window = nullptr;

	// Number of particles in screen at the moment
	const int m_num_p;

	float m_dt = 0; // Delta time between frames
	float m_t_last = 0; // Time of last frame
	float m_time_press_t = 0; // Last press of togle shadow map

	bool m_shadowsEnabled; // shadow map activated?

	bool m_userInputEnabled = true; // complex user input activated?

	Shader m_shaders[3]; // [0] Particle, [1] BB, [2] ShadowMap
	Camera m_camera = Camera(glm::vec3(0.5f, 0.5f, 3.0f)); // Camera
	
	glm::vec3 m_lightPosition = glm::vec3(0.5f, 1.0f, 3.0f); 
	glm::vec3 m_lightColor = glm::vec3(0.6f, 0.6f, 0.6f);
	glm::vec3 m_ambientLight = glm::vec3(0.2f, 0.2f, 0.2f);

	glm::mat4 m_particleModel; // Model matrix of the particles
	glm::vec3 m_particleScale = glm::vec3(5e-3f); // particles scaling factor

	bool m_firstMouse = true; // Has the mouse moved yet?
	float m_lastX, m_lastY; // last mouse positions

	int m_SCR_WIDTH, m_SCR_HEIGHT; // size of the screen
	int m_shadowTex_w = 2048, m_shadowTex_h = 2048; // size of the shadow map

	int m_trianglesToDraw = 36; // for cube

	// OpenGL Buffers
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
