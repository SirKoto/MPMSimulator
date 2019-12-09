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

constexpr GLfloat Pvertices[] = {
	// Back face
 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, -1.0f, // top-right             
 1.0f, 0.0f, 0.0f,  1.0f, -1.0f, -1.0f, // bottom-right  
 0.0f, 0.0f, 0.0f,  -1.0f, -1.0f, -1.0f, // Bottom-left
 0.0f, 0.0f, 0.0f,  -1.0f, -1.0f, -1.0f, // bottom-left                
 0.0f,  1.0f, 0.0f, -1.0f, 1.0f, -1.0f, // top-left
 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, -1.0f, // top-right

// Front face
 1.0f, 0.0f,  1.0f,  1.0f, -1.0f, 1.0f, // bottom-right
 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f, // top-right
 0.0f, 0.0f,  1.0f,  -1.0f, -1.0f, 1.0f, // bottom-left	         
 0.0f,  1.0f,  1.0f, -1.0f, 1.0f, 1.0f, // top-left
 0.0f, 0.0f,  1.0f,  -1.0f, -1.0f, 1.0f, // bottom-left
 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f, // top-right

// Left face
 0.0f,  1.0f, 0.0f,  -1.0f, 1.0f, -1.0f, // top-left       
 0.0f, 0.0f, 0.0f,  -1.0f, -1.0f, -1.0f, // bottom-left
 0.0f,  1.0f,  1.0f,  -1.0f, 1.0f, 1.0f, // top-right
 0.0f, 0.0f,  1.0f,  -1.0f, -1.0f, 1.0f, // bottom-right
 0.0f,  1.0f,  1.0f,  -1.0f, 1.0f, 1.0f, // top-right
 0.0f, 0.0f, 0.0f,  -1.0f, -1.0f, -1.0f, // bottom-left

// Right face
 1.0f, 0.0f, 0.0f,  1.0f, -1.0f, -1.0f, // bottom-right  
 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, -1.0f, // top-right 
 1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, // top-left
 1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, // top-left
 1.0f, 0.0f,  1.0f,  1.0f, -1.0f, 1.0f, // bottom-left
 1.0f, 0.0f, 0.0f,  1.0f, -1.0f, -1.0f, // bottom-right

// Bottom face          
 1.0f, 0.0f, 0.0f,  1.0f, -1.0f, -1.0f, // top-left
 1.0f, 0.0f,  1.0f,  1.0f, -1.0f, 1.0f, // bottom-left
 0.0f, 0.0f, 0.0f,  -1.0f, -1.0f, -1.0f, // top-right
 0.0f, 0.0f,  1.0f,  -1.0f, -1.0f, 1.0f, // bottom-right
 0.0f, 0.0f, 0.0f,  -1.0f, -1.0f, -1.0f, // top-right
 1.0f, 0.0f,  1.0f,  1.0f, -1.0f, 1.0f, // bottom-left

// Top face
 1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, // bottom-right
 1.0f,  1.0f, 0.0f,  1.0f, 1.0f, -1.0f, // top-right
 0.0f,  1.0f, 0.0f,  -1.0f, 1.0f, -1.0f, // top-left                 
 0.0f,  1.0f, 0.0f,  -1.0f, 1.0f, -1.0f,  // top-left
 0.0f,  1.0f,  1.0f,  -1.0f, 1.0f, 1.0f, // bottom-left  
 1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, // bottom-right      
};

std::vector<GLfloat> genIcosphere() {
	constexpr float PI = 3.14159265358979323846f;
	constexpr float H_ANGLE = (PI / 180) * 72;         // 72 degree = 360 / 5
	float V_ANGLE = std::atan(1.0f / 2.0f);  // elevation = 26.565 degree

	std::vector<glm::vec3> vertices(12);
	float z = std::sin(V_ANGLE), xy = std::cos(V_ANGLE); // elevation
	float hAngle1 = -PI / 2 - H_ANGLE / 2; // start at the second row. -126 degrees
	float hAngle2 = -PI / 2; // -90 degrees. third row

	vertices[0] = glm::vec3(0, 0, 1); // topmost vertex of radius 1

	// all the vertices (10) between the top and the bottom
	for (int i = 1; i <= 5; ++i)
	{
		int j = i + 5; // the simetric index

		vertices[i] = glm::vec3(
			xy * std::cos(hAngle1),
			xy * std::sin(hAngle1),
			z
		);

		vertices[j] = glm::vec3(
			xy * std::cos(hAngle2),
			xy * std::sin(hAngle2),
			-z
		);
		hAngle1 += H_ANGLE;
		hAngle2 += H_ANGLE;
	}

	vertices[11] = glm::vec3(0, 0, -1);

	std::vector<int> indices;
	indices.reserve(20 * 3);
	auto addIndices = [&indices](int a, int b, int c)
	{
		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
	};

	int v0, v1, v2, v3, v4, v11;
	v0 = 0;
	v11 = 11;
	for (int i = 1; i <= 5; ++i)
	{
		// 4 vertices in the 2nd row
		v1 = i;
		v3 = i + 5;
		if (i < 5) {
			v2 = i + 1;
			v4 = i + 6;
		}
		else
		{
			v2 = 1;
			v4 = 6;
		}

		// 4 new triangles per iteration
		addIndices(v0, v1, v2);
		addIndices(v1, v3, v2);
		addIndices(v2, v3, v4);
		addIndices(v3, v11, v4);
	}

	std::vector<glm::vec3> normals(indices.size());
	for (int i = 0; i < indices.size(); i += 3)
	{
		glm::vec3 ab = vertices[indices[i + 1]] - vertices[indices[i]],
			ac = vertices[indices[i + 2]] - vertices[indices[i]];
		glm::vec3 normal = glm::normalize(glm::cross(ab, ac));
		if (glm::dot(normal, vertices[indices[i + 1]]) < 0)
		{
			// invert vertices and normal
			normal = -normal;
			std::swap(indices[i + 1], indices[i + 2]);
		}


		normals[i] = normal;
		normals[i+1] = normal;
		normals[i+2] = normal;
	}

	std::vector<glm::vec3> normals2(vertices.size(), glm::vec3(0));
	// smoth normals
	for (int i = 0; i < indices.size(); ++i)
	{
		normals2[indices[i]] += normals[i];
	}
	for (glm::vec3& v : normals2)
	{
		v = glm::normalize(v);
	}

	std::vector<GLfloat> res(2 * 3 * indices.size());
	for (int i = 0; i < indices.size(); ++i)
	{
		int p = 6 * i;
		res[p + 0] = vertices[indices[i]].x;
		res[p + 1] = vertices[indices[i]].y;
		res[p + 2] = vertices[indices[i]].z;

		res[p + 3] = normals2[indices[i]].x;
		res[p + 4] = normals2[indices[i]].y;
		res[p + 5] = normals2[indices[i]].z;

	}
	return res;
}
SimVisualizer::SimVisualizer(int num_particles, bool shadows,
	int width, int heigth) :
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

void SimVisualizer::setShadowMapRes(int w, int h)
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
	m_particleModel = glm::translate(m_particleModel, glm::vec3(-0.5f));
}

void SimVisualizer::reloadShaders()
{
	if (m_shadowsEnabled)
	{

		// enable particle shader
		m_shaders[0] = Shader("shadersShadows/shaderPoint.vert", "shadersShadows/shaderPoint.frag");
		m_shaders[0].use();
		m_shaders[0].setInt("shadowMap", 0); // set shadowMap uniform sampler2D

		// enable BB shader
		m_shaders[1] = Shader("shadersShadows/shaderBB.vert", "shadersShadows/shaderBB.frag");
		m_shaders[1].use();
		m_shaders[1].setInt("shadowMap", 0); // set shadowMap uniform

		// enable ShadowMap shader
		m_shaders[2] = Shader("shadersShadows/shaderShadowMap.vert", "shadersShadows/shaderShadowMap.frag");
		m_shaders[2].use();
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

	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * static_cast<size_t>(m_num_p) * sizeof(float), pos);

}

void SimVisualizer::updateParticlePositions(const FrameSBF<float>& frame)
{
	updateParticlePositions(frame.const_ptr());
}

void SimVisualizer::updateParticlesColor(const float* color)
{
	glBindVertexArray(m_VAO_particles);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_particles[1]);

	glBufferData(GL_ARRAY_BUFFER, 3 * static_cast<size_t>(m_num_p) * sizeof(float), color, GL_STATIC_DRAW);
}

void SimVisualizer::draw()
{
	updateDT();

	if (m_userInputEnabled)
	{
		processKeyboardInput();
	}
	else
	{
		processKeyboardInputLess();
	}

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

	glBindVertexArray(0);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void SimVisualizer::enableUserInput(bool enable)
{
	if (m_userInputEnabled == enable)
		return;

	if (enable)
	{
		setCallbacks();
	}
	else
	{
		unsetCallbacks();
	}

	m_userInputEnabled = enable;
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

	glfwSwapInterval(0);

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

	// store the window with the viewer class
	glfwSetWindowUserPointer(m_window, this);

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

void SimVisualizer::updateDT()
{
	float currentFrame = static_cast<float>(glfwGetTime());
	m_dt = currentFrame - m_t_last;
	m_t_last = currentFrame;
}

void SimVisualizer::processKeyboardInput()
{
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(m_window, GLFW_TRUE);

	float d = GLFW_PRESS == glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) ? 0.3f : 1.0f;


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

	if (m_time_press_t > 0) {
		m_time_press_t -= m_dt;
	}
	else if (glfwGetKey(m_window, GLFW_KEY_T) == GLFW_PRESS)
	{
		m_time_press_t = 0.35f;
		m_shadowsEnabled = !m_shadowsEnabled;
		reloadShaders();
	}

	if (glfwGetKey(m_window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
	{
		glm::vec3 tmp = glm::vec3(m_dt * d * 5e-3f);
		m_particleScale += tmp;
		m_particleScale = glm::clamp(m_particleScale, 1e-7f, 1e-2f);
		setScaleParticles(m_particleScale);
	}
	if (glfwGetKey(m_window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
	{
		m_particleScale -= glm::vec3(m_dt * d * 5e-3f);
		m_particleScale = glm::clamp(m_particleScale, 1e-7f, 1e-2f);
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

	if (glfwGetKey(m_window, GLFW_KEY_F) == GLFW_PRESS)
	{
		m_lightPosition = m_camera.m_position;
	}

	processKeyboardInputCallbacks();

}

void SimVisualizer::processKeyboardInputLess()
{
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

void SimVisualizer::setKeyCallback(KEYS key, std::function<void()> f)
{
	f_call[static_cast<int>(key)] = f;
}

void SimVisualizer::temptateEvents()
{
	if (m_userInputEnabled)
	{
		processKeyboardInput();
	}
	else
	{
		processKeyboardInputLess();
	}

	glfwPollEvents();
}

void SimVisualizer::setCallbacks()
{

	auto f_framebuffer_callback = [](GLFWwindow* w, int width, int height)
	{
		static_cast<SimVisualizer*>(glfwGetWindowUserPointer(w))->framebuffer_size_callback(w, width, height);
	};

	glfwSetFramebufferSizeCallback(m_window, f_framebuffer_callback);

	setMouseInteractive(true);
}

void SimVisualizer::unsetCallbacks()
{
	setMouseInteractive(false);
	glfwSetFramebufferSizeCallback(m_window, NULL);
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

void SimVisualizer::processKeyboardInputCallbacks()
{
	if (f_call[static_cast<int>(KEYS::U)] &&
		glfwGetKey(m_window, GLFW_KEY_U) == GLFW_PRESS)
	{
		f_call[static_cast<int>(KEYS::U)]();
	}

	if (f_call[static_cast<int>(KEYS::I)] &&
		glfwGetKey(m_window, GLFW_KEY_I) == GLFW_PRESS)
	{
		f_call[static_cast<int>(KEYS::I)]();
	}

	if (f_call[static_cast<int>(KEYS::O)] &&
		glfwGetKey(m_window, GLFW_KEY_O) == GLFW_PRESS)
	{
		f_call[static_cast<int>(KEYS::O)]();
	}

	if (f_call[static_cast<int>(KEYS::P)] &&
		glfwGetKey(m_window, GLFW_KEY_P) == GLFW_PRESS)
	{
		f_call[static_cast<int>(KEYS::P)]();
	}

	if (f_call[static_cast<int>(KEYS::K)] &&
		glfwGetKey(m_window, GLFW_KEY_K) == GLFW_PRESS)
	{
		f_call[static_cast<int>(KEYS::K)]();
	}

	if (f_call[static_cast<int>(KEYS::L)] &&
		glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS)
	{
		f_call[static_cast<int>(KEYS::L)]();
	}

	if (f_call[static_cast<int>(KEYS::ENTER)] &&
		glfwGetKey(m_window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		f_call[static_cast<int>(KEYS::ENTER)]();
	}
}

/**
Attribs are [0] vertex, [1] color, [2] offset, [3] normal
*/
void SimVisualizer::initArraysParticles()
{
	float* tmp = new float[(3 * static_cast<size_t>(m_num_p))];
	std::memset(tmp, 0, 3 * static_cast<size_t>(m_num_p) * sizeof(float));

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
	std::vector<GLfloat> r = genIcosphere();
	// buffer static data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * r.size(), &r[0] , GL_STATIC_DRAW);
	m_trianglesToDraw = static_cast<int>(r.size()) / (3 * 2);

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

	if (m_shadowsEnabled)
	{
		const glm::mat4 lightProjection = glm::perspective(glm::radians(72.0f), static_cast<float>(m_shadowTex_w) / m_shadowTex_h, 1.522f, 3.522f);//glm::ortho(-0.5f, 0.5f, -0.6f, 0.6f, 1.522f, 3.522f);//
		const glm::mat4 lightView = glm::lookAt(m_lightPosition, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
		const glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		m_shaders[0].use();
		m_shaders[0].setMat4("lightSpaceMatrix", lightSpaceMatrix);

		m_shaders[1].use();
		m_shaders[1].setMat4("lightSpaceMatrix", lightSpaceMatrix);

		m_shaders[2].use();
		m_shaders[2].setMat4("lightSpaceMatrix", lightSpaceMatrix);

	}
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

void SimVisualizer::drawParticles(bool enable) const
{	
	glBindVertexArray(m_VAO_particles);
	if (m_shadowsEnabled && enable)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_depthMapTex);
	}

	// Draw instancing different offsets with the same model
	glDrawArraysInstanced(GL_TRIANGLES, 0, m_trianglesToDraw, static_cast<GLsizei>(m_num_p));
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
	drawParticles(false);
	glCullFace(GL_BACK);

	// unbind and set normal viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, static_cast<GLsizei>(m_SCR_WIDTH), static_cast<GLsizei>(m_SCR_HEIGHT));
}
