#include "SimVisualizer.h"


#include <glm/gtc/matrix_transform.hpp>


constexpr GLfloat vertices[] = {
	// Back face
 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f, // top-right             
 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, // bottom-right  
 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, // Bottom-left
 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, // bottom-left                
 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f, // top-left
 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f, // top-right

// Front face
 1.0f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // bottom-right
 1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top-right
 0.0f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // bottom-left	         
 0.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top-left
 0.0f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // bottom-left
 1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top-right

// Left face
 0.0f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // top-left       
 0.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // bottom-left
 0.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f, // top-right
 0.0f, 0.0f,  1.0f,  -1.0f, 0.0f, 0.0f, // bottom-right
 0.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f, // top-right
 0.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // bottom-left

// Right face
 1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom-right  
 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // top-right 
 1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, // top-left
 1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, // top-left
 1.0f, 0.0f,  1.0f,  1.0f, 0.0f, 0.0f, // bottom-left
 1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom-right

// Bottom face          
 1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // top-left
 1.0f, 0.0f,  1.0f,  0.0f, -1.0f, 0.0f, // bottom-left
 0.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // top-right
 0.0f, 0.0f,  1.0f,  0.0f, -1.0f, 0.0f, // bottom-right
 0.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // top-right
 1.0f, 0.0f,  1.0f,  0.0f, -1.0f, 0.0f, // bottom-left

// Top face
 1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
 1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // top-right
 0.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // top-left                 
 0.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // top-left
 0.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-left  
 1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right      
};



SimVisualizer::SimVisualizer(size_t num_particles, bool shadows,
	size_t width, size_t heigth) :
	m_num_p(num_particles), m_shadowsEnabled(shadows),
	m_SCR_WIDTH(width), m_SCR_HEIGHT(heigth)
{
	if (!initGLFW())
	{
		m_ERROR = true;
		return;
	}
	if (!initOpenGL())
	{
		m_ERROR = true;
		return;
	}
}

SimVisualizer::~SimVisualizer()
{
	glfwTerminate();
}

void SimVisualizer::SetClearColor(glm::vec3 rgb)
{
	glClearColor(rgb.r, rgb.g, rgb.b, 1.0f);
}

void SimVisualizer::setShadowMapRes(size_t w, size_t h)
{
	m_shadowTex_w = w;
	m_shadowTex_h = h;
	glBindTexture(GL_TEXTURE_2D, m_depthMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
		static_cast<GLsizei>(m_shadowTex_w),
		static_cast<GLsizei>(m_shadowTex_h),
		0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void SimVisualizer::setScaleParticles(glm::vec3 scale)
{
	m_particleModel = glm::scale(glm::mat4(1.0f), scale);
}

void SimVisualizer::reloadShaders()
{
	if (m_shadowsEnabled)
	{
		const glm::mat4 lightProjection = glm::ortho(-0.5f, 0.5f, -0.6f, 0.6f, 1.522f, 3.522f);//glm::perspective(glm::radians(72.0f), static_cast<float>(utils::SHADOW_WIDTH) / utils::SHADOW_HEIGHT, 0.01f, 3.5f);
		const glm::mat4 lightView = glm::lookAt(m_lightPosition, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
		const glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		// enable particle shader
		m_shaders[0] = Shader("shadersShadows/shaderPoint.vert", "shadersShadows/shaderPoint.frag");
		m_shaders[0].use();
		m_shaders[0].setInt("shadowMap", 0); // set shadowMap uniform sampler2D
		m_shaders[0].setMat4("lightSpaceMatrix", lightSpaceMatrix);

		// enable BB shader
		m_shaders[1] = Shader("shadersShadows/shaderBB.vert", "shadersShadows/shaderBB.frag");
		m_shaders[1].use();
		m_shaders[1].setInt("shadowMap", 0); // set shadowMap uniform
		m_shaders[1].setMat4("lightSpaceMatrix", lightSpaceMatrix);

		// enable ShadowMap shader
		m_shaders[2] = Shader("shadersShadows/shaderShadowMap.vert", "shadersShadows/shaderShadowMap.frag");
		m_shaders[2].use();
		m_shaders[2].setMat4("lightSpaceMatrix", lightSpaceMatrix);

	}
	else
	{
		// enable particle shader
		m_shaders[0] = Shader("shaders/shaderPoint.vert", "shaders/shaderPoint.frag");

		m_shaders[1] = Shader("shaders/shaderBB.vert", "shaders/shaderBB.frag");

	}

	updateUniforms();
}

bool SimVisualizer::ErrorHappened() const
{
	return m_ERROR;
}

void SimVisualizer::updateParticlePositions(const float* pos)
{
	glBindVertexArray(m_VAO_particles);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_particles[0]);

	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * m_num_p * sizeof(float), pos);

}

void SimVisualizer::updateParticlePositions(const FrameSBF<float>& frame)
{
	updateParticlePositions(frame.const_ptr());
}

void SimVisualizer::updateParticlesColor(const float* color)
{
	glBindVertexArray(m_VAO_particles);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_particles[1]);

	glBufferData(GL_ARRAY_BUFFER, 3 * m_num_p * sizeof(float), color, GL_STATIC_DRAW);
}

void SimVisualizer::draw()
{
	// Update uniforms of all shaders
	updateUniforms();

	if (m_shadowsEnabled)
	{
		drawShadowMap();
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	m_shaders[1].use();
	drawBB();

	m_shaders[0].use();
	drawParticles();

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

bool SimVisualizer::shouldApplicationClose()
{
	return glfwWindowShouldClose(m_window);
}

bool SimVisualizer::initGLFW()
{
	glfwInit();
	// Set OpenGL to 3.3... If in the future compute 
	// shaders are used.. version 4.4 is needed
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(
		static_cast<int>(m_SCR_WIDTH), 
		static_cast<int>(m_SCR_HEIGHT), 
		"3D Simulation Visualizer", NULL, NULL);

	if (m_window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return false;
	}

	// set viewport
	glViewport(0, 0, 
		static_cast<GLsizei>(m_SCR_WIDTH), 
		static_cast<GLsizei>(m_SCR_HEIGHT));

	return true;
}

bool SimVisualizer::initOpenGL()
{
	SetClearColor(glm::vec3(0.2f, 0.3f, 0.3f));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// Triangles must be counterclockwise
	glFrontFace(GL_CCW);
	// set callbacks
	setCallbacks();

	// default particle size
	setScaleParticles(m_particleScale);

	initArraysParticles();
	initArraysBB();
	initFBOShadows();

	// shaders
	reloadShaders();

	return true;
}

void SimVisualizer::setMouseInteractive(bool interactive)
{
	if (interactive)
	{
		auto f_cursorPositionCallback = [](GLFWwindow* window, double xpos, double ypos)
		{
			static_cast<SimVisualizer*>(glfwGetWindowUserPointer(window))->mouse_callback(window, xpos, ypos);
		};

		glfwSetCursorPosCallback(m_window, f_cursorPositionCallback);

		auto f_scrollCallback = [](GLFWwindow* window, double xoffset, double yoffset)
		{
			static_cast<SimVisualizer*>(glfwGetWindowUserPointer(window))->scroll_callback(window, xoffset, yoffset);
		};

		glfwSetScrollCallback(m_window, f_scrollCallback);
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else
	{
		glfwSetScrollCallback(m_window, NULL);
		glfwSetCursorPosCallback(m_window, NULL);

		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void SimVisualizer::processKeyboardInput()
{
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(m_window, GLFW_TRUE);

	float d = GLFW_PRESS == glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) ? 0.3f : 1.0f;

	if (glfwGetKey(m_window, GLFW_KEY_ENTER) == GLFW_PRESS)
		m_enterPressed = true;

	float cameraSpeed = 2.5f * m_dt;
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
		m_camera.ProcessKeyboard(Camera::Camera_Movement::FORWARD, m_dt * d);
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
		m_camera.ProcessKeyboard(Camera::Camera_Movement::BACKWARD, m_dt * d);
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
		m_camera.ProcessKeyboard(Camera::Camera_Movement::RIGHT, m_dt * d);
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
		m_camera.ProcessKeyboard(Camera::Camera_Movement::LEFT, m_dt * d);

	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
		reloadShaders();

	if (glfwGetKey(m_window, GLFW_KEY_T) == GLFW_PRESS)
	{
		m_shadowsEnabled = !m_shadowsEnabled;
		reloadShaders();
	}

	if (glfwGetKey(m_window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
	{
		glm::vec3 tmp = glm::vec3(m_dt * d * 5e-1f);
		m_particleScale += tmp;
		m_particleScale = glm::clamp(m_particleScale, 1e-6f, 1.0f);
		setScaleParticles(m_particleScale);
	}
	if (glfwGetKey(m_window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
	{
		m_particleScale -= glm::vec3(m_dt * d * 5e-1f);
		m_particleScale = glm::clamp(m_particleScale, 1e-6f, 1.0f);
		setScaleParticles(m_particleScale);
	}

	if (glfwGetKey(m_window, GLFW_KEY_M) == GLFW_PRESS)
	{
		setMouseInteractive(true);
	}
	if (glfwGetKey(m_window, GLFW_KEY_N) == GLFW_PRESS)
	{
		setMouseInteractive(false);
	}

}

void SimVisualizer::setCallbacks()
{
	// store the window with the viewer class
	glfwSetWindowUserPointer(m_window, this);

	auto f_framebuffer_callback = [](GLFWwindow* w, int width, int height)
	{
		static_cast<SimVisualizer*>(glfwGetWindowUserPointer(w))->framebuffer_size_callback(w, width, height);
	};

	glfwSetFramebufferSizeCallback(m_window, f_framebuffer_callback);

	setMouseInteractive(true);
}

void SimVisualizer::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	m_SCR_WIDTH = width;
	m_SCR_HEIGHT = height;
}

void SimVisualizer::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float xposf = static_cast<float>(xpos);
	float yposf = static_cast<float>(ypos);

	if (m_firstMouse)
	{
		m_lastX = xposf;
		m_lastY = yposf;

		m_firstMouse = false;
	}

	float xoffset = xposf - m_lastX;
	float yoffset = yposf - m_lastY;

	m_lastX = xposf;
	m_lastY = yposf;

	m_camera.ProcessMouseMovement(xoffset, yoffset);
}

void SimVisualizer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	m_camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

/**
Attribs are [0] vertex, [1] color, [2] offset, [3] normal
*/
void SimVisualizer::initArraysParticles()
{
	float* tmp = new float[(3 * m_num_p)];
	std::memset(tmp, 0, 3 * m_num_p * sizeof(float));

	glGenVertexArrays(1, &m_VAO_particles);
	glBindVertexArray(m_VAO_particles);

	glGenBuffers(3, m_VBO_particles);

	// VBO 0 is the position of the particles. or offset of the cubes
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_particles[0]);
	// must buffer junk data because of dynamic array
	glBufferData(GL_ARRAY_BUFFER, m_num_p * (3 * sizeof(float)), tmp, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	// VBO 1 is color of the particle to draw
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_particles[1]);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);

	// VBO 2 is the vertices and the normal
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_particles[2]);
	// buffer static data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(3);


	delete[] tmp;
}

// ATTRIBS [0] vertex [1] normal
void SimVisualizer::initArraysBB()
{
	glGenVertexArrays(1, &m_VAO_BB);
	glBindVertexArray(m_VAO_BB);

	glGenBuffers(1, &m_VBO_BB);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_BB);
	// Buffer vertices position of the BB
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void SimVisualizer::initFBOShadows()
{

	// Generate the framebuffer
	glGenFramebuffers(1, &m_depthFBO);

	// texture to store the depth
	glGenTextures(1, &m_depthMapTex);
	glBindTexture(GL_TEXTURE_2D, m_depthMapTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 
		static_cast<GLsizei>(m_shadowTex_w), 
		static_cast<GLsizei>(m_shadowTex_h), 
		0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
	// Bind texture to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMapTex, 0);
	// prevent from writting or reading the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "ERROR CREATING FRAMEBUFFER" << std::endl;
	// unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SimVisualizer::updateUniforms()
{
	const glm::mat4 projection = glm::perspective(glm::radians(m_camera.m_zoom), static_cast<float>(m_SCR_WIDTH) / m_SCR_HEIGHT, 0.1f, 10.0f);
	const glm::mat4 projectionView = projection * m_camera.GetViewMatrix();
	for (int i = 0; i < 2; i++) setUniforms(m_shaders[i], projectionView);

	updateModelMatrix();
}

void SimVisualizer::updateModelMatrix()
{
	// modify particle shader
	m_shaders[0].use();
	m_shaders[0].setMat4("model", m_particleModel);

	if (m_shadowsEnabled)
	{
		// modify shadow shader
		m_shaders[2].use();
		m_shaders[2].setMat4("model", m_particleModel);
	}
}

void SimVisualizer::setUniforms(Shader& s, const glm::mat4& projectionView)
{
	s.use();

	s.setMat4("view", m_camera.GetViewMatrix());
	s.setVec3("camera", m_camera.m_position);
	s.setVec3("lightPos", m_lightPosition);
	s.setVec3("lightColor", m_lightColor);
	s.setVec3("ambientLight", m_ambientLight);
	s.setMat4("projectionView", projectionView);
}

void SimVisualizer::drawBB() const
{
	glBindVertexArray(m_VAO_BB);
	
	if (m_shadowsEnabled)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_depthMapTex);
	}

	glCullFace(GL_FRONT);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glCullFace(GL_BACK);
}

void SimVisualizer::drawParticles() const
{	
	glBindVertexArray(m_VAO_particles);
	if (m_shadowsEnabled)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_depthMapTex);
	}

	// Draw instancing different offsets with the same model
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, static_cast<GLsizei>(m_num_p));
}

void SimVisualizer::drawShadowMap() const
{
	// Set framebuffer
	glViewport(0, 0, static_cast<GLsizei>(m_shadowTex_w), static_cast<GLsizei>(m_shadowTex_h));
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);
	// clear depth
	glClear(GL_DEPTH_BUFFER_BIT);

	glCullFace(GL_FRONT);
	m_shaders[2].use(); // draw with shadowMap shader
	drawParticles();
	glCullFace(GL_BACK);

	// unbind and set normal viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, static_cast<GLsizei>(m_SCR_WIDTH), static_cast<GLsizei>(m_SCR_HEIGHT));
}
