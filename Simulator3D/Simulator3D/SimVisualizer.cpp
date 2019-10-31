#include "SimVisualizer.h"


#include <glm/gtc/matrix_transform.hpp>
	
#include <functional>   // std::bind

#include "Utils.h"


SimVisualizer::SimVisualizer(size_t num_particles, bool shadows,
	size_t width, size_t heigth) :
	m_num_p(num_particles), m_shadowsEnabled(shadows),
	m_SCR_WIDTH(width), m_SCR_HEIGHT(heigth)
{
	if (!initGLFW())
	{
		ERROR = true;
		return;
	}
	if (!initOpenGL())
	{
		ERROR = true;
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

bool SimVisualizer::ErrorHappened()
{
	return ERROR;
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

	initArraysParticles();
	initArraysBB();
	initFBOShadows();

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
	float* tmp = new float[3 * m_num_p];
	glGenVertexArrays(1, &m_VAO_particles);
	glBindVertexArray(m_VAO_particles);

	glGenBuffers(3, m_VBO_particles);

	// VBO 0 is the position of the particles. or offset of the cubes
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_particles[0]);
	// must buffer junk data because of dynamic array
	glBufferData(GL_ARRAY_BUFFER, m_num_p * 3 * sizeof(float), tmp, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	// VBO 1 is color of the particle to draw
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_particles[1]);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);

	// VBO 2 is the vertices and the normal
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_particles[2]);
	// buffer static data
	glBufferData(GL_ARRAY_BUFFER, sizeof(utils::vertices), utils::vertices, GL_STATIC_DRAW);

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(utils::vertices), utils::vertices, GL_STATIC_DRAW);

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
		MSG("Error creating frameBuffer");
	// unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
