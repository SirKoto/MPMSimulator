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

	m_window = glfwCreateWindow(m_SCR_WIDTH, m_SCR_HEIGHT, "3D Simulation Visualizer", NULL, NULL);

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
	glViewport(0, 0, m_SCR_WIDTH, m_SCR_HEIGHT);

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
	using namespace std::placeholders;

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