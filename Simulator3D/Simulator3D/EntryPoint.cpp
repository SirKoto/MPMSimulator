
#pragma once

#include "Utils.h"

#include <Windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <random>

#include "Shader.h"
#include "Simulator_3D.h"
#include "Camera.h"

#include "IO/WriteSBF.h"
#include "IO/ReadSBF.h"


// #define PRINT_IMAGES_FLAG
#define WRITE_DATA_SBF
#define SHADOWS

struct ParticlePos
{
	float x, y;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void processInputLess(GLFWwindow* window);


Shader shader, shaderBB, shaderShadow;
Camera camera(glm::vec3(0.5f, 0.5f, 5.0f));
glm::vec3 lightPosition(0.5f, 1.0f, 3.0f);
glm::vec3 lightColor(0.6f, 0.6f, 0.6f);
glm::vec3 ambientLight(0.2f, 0.2f, 0.2f);

bool firstMouse = true;
bool doSimulation = false;
float lastX, lastY;


GLuint VAO_particles, VBO_particles[2];
GLuint VAO_BB, VBO_BB[2];

#ifdef SHADOWS
GLuint depthFBO;
GLuint depthMapTex;
#endif

bool initGLFW(GLFWwindow *&window)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(utils::SCR_WIDTH, utils::SCR_HEIGHT, "2D_SIM", NULL, NULL);

	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return false;
	}

	glViewport(0, 0, utils::SCR_WIDTH, utils::SCR_HEIGHT);

	return true;
}

void setCallbacks(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//Camputa mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void deactivateCallbacks(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, NULL);
	glfwSetCursorPosCallback(window, NULL);
	glfwSetScrollCallback(window, NULL);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // disable dissapearing cursor
}

glm::mat4 getParticleModel()
{
	return glm::scale(glm::mat4(1.0f), glm::vec3(utils::particleSize));
}

void initArraysParticles(GLuint& VAO, GLuint* VBO, float* &positions, glm::vec3* &colors)
{
	positions = new float[utils::maxParticles * 3];
	colors = new glm::vec3[utils::maxParticles];

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(3, VBO); 
	//Attribs [0] vertex, [1] color, [2] offset, [3] normal
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, utils::maxParticles * 3 * sizeof(float), positions, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, utils::maxParticles * sizeof(glm::vec3), colors, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(utils::vertices), utils::vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(3);

#ifdef SHADOWS
	shader = Shader("shadersShadows/shaderPoint.vert", "shadersShadows/shaderPoint.frag");
	shader.use();
	shader.setInt("shadowMap", 0);
#else
	shader = Shader("shaders/shaderPoint.vert", "shaders/shaderPoint.frag");
	shader.use();
#endif
	shader.setMat4("model", getParticleModel());
}



void initArraysBB(GLuint& VAO, GLuint* VBO)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, VBO); 

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(utils::vertices), utils::vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

#ifdef SHADOWS
	shaderBB = Shader("shadersShadows/shaderBB.vert", "shadersShadows/shaderBB.frag");
#else
	shaderBB = Shader("shaders/shaderBB.vert", "shaders/shaderBB.frag");
#endif

	const glm::vec3 colorBB = glm::vec3(1.0f, 0.0, 0.0f);
	shaderBB.use();
	shaderBB.setVec3("colorBBox", colorBB);

#ifdef SHADOWS
	shaderBB.setInt("shadowMap", 0);
#endif
}

#ifdef SHADOWS
void initFBOShadows() {

	// Generate the framebuffer
	glGenFramebuffers(1, &depthFBO);

	// texture to store the depth
	glGenTextures(1, &depthMapTex);
	glBindTexture(GL_TEXTURE_2D, depthMapTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, utils::SHADOW_WIDTH, utils::SHADOW_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
	// Bind texture to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTex, 0);
	// prevent from writting or reading the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		MSG("Error creating frameBuffer");
	// unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	const glm::mat4 lightProjection = glm::ortho(-0.5f, 0.5f, -0.6f, 0.6f, 1.522f, 3.522f);//glm::perspective(glm::radians(72.0f), static_cast<float>(utils::SHADOW_WIDTH) / utils::SHADOW_HEIGHT, 0.01f, 3.5f);
	const glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	shaderShadow = Shader("shadersShadows/shaderShadowMap.vert", "shadersShadows/shaderShadowMap.frag");
	shaderShadow.use();
	// Set light matrix
	shaderShadow.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shaderShadow.setMat4("model", getParticleModel());
	// set light matrix to all the other shaders
	shaderBB.use();
	shaderBB.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	shader.use();
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

}
#endif

void setUniforms(Shader s, const glm::mat4& projectionView)
{
	s.use();
	
	s.setMat4("projectionView", projectionView);
	s.setMat4("view", camera.GetViewMatrix());
	s.setVec3("camera", camera.m_position);
	s.setVec3("lightPos", lightPosition);
	s.setVec3("lightColor", lightColor);
	s.setVec3("ambientLight", ambientLight);

}



void drawBBFilled(const GLuint VAO, const Shader& shr = shaderBB)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	shr.use();

	glBindVertexArray(VAO);

#ifdef SHADOWS
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMapTex);
#endif

	glCullFace(GL_FRONT);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glCullFace(GL_BACK);

}

unsigned int updateParticles(const GLuint VAO, const GLuint VBO, const Simulator_3D& sim, float* particleDump)
{

	unsigned int n = sim.dumpPositionsNormalized(particleDump);

	glBindVertexArray(VAO);

	// update positions
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * (3 * sizeof(float)), particleDump);

	return n;
}

void drawParticles(const GLuint VAO, const unsigned int num_particles, const Shader& shr = shader)
{
	// Draw
	glBindVertexArray(VAO);
	shr.use();

#ifdef SHADOWS
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMapTex);
#endif

	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, num_particles);
}

#ifdef SHADOWS
void drawShadowMap(const int num_particles)
{
	// Set framebuffer
	glViewport(0, 0, utils::SHADOW_WIDTH, utils::SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	// clear depth
	glClear(GL_DEPTH_BUFFER_BIT);

	glCullFace(GL_FRONT);
	drawParticles(VAO_particles, num_particles, shaderShadow);
	glCullFace(GL_BACK);

	// unbind and set normal viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, utils::SCR_WIDTH, utils::SCR_HEIGHT);
}
#endif

void updateUniforms()
{
	const glm::mat4 projection = glm::perspective(glm::radians(camera.m_zoom), static_cast<float>(utils::SCR_WIDTH) / utils::SCR_HEIGHT, 0.1f, 10.0f);
	const glm::mat4 projectionView = projection * camera.GetViewMatrix();
	setUniforms(shader, projectionView);
	setUniforms(shaderBB, projectionView);
}

void draw(const Simulator_3D& sim, float* buff)
{
	const int n = updateParticles(VAO_particles, VBO_particles[0], sim, buff);

	updateUniforms();
#ifdef SHADOWS
	// Draw texture of shadow map
	drawShadowMap(n);
#endif
	// Clear before drawing
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawBBFilled(VAO_BB);
	drawParticles(VAO_particles, n);
}

int main()
{
	GLFWwindow* window = NULL;
	if (!initGLFW(window))
		return -1;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Clear color state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	setCallbacks(window);

	float* p_pos;
	glm::vec3* p_col;
	// VBO[0] := positions, VBO[1] := color
	initArraysParticles(VAO_particles, VBO_particles, p_pos, p_col);

	initArraysBB(VAO_BB, VBO_BB);

#ifdef SHADOWS
	initFBOShadows();
#endif

	int n_particles = utils::maxParticles;

	// Create simulator and add points
	Simulator_3D sim;
	{
		// add random particles

		std::mt19937 mt_rng(42);
		std::uniform_real_distribution<float> disX(0.1f, 0.9f);
		std::uniform_real_distribution<float> disZ(0.3f, 0.7f);
		std::uniform_real_distribution<float> disY(0.4f, 0.8f);

		float dy = (0.8f - 0.4f) / 3;
		for (int i = 0; i < n_particles; ++i)
		{
			float x = disX(mt_rng);
			float y = disY(mt_rng);
			float z = disZ(mt_rng);
			sim.addParticleNormalized(glm::vec3(x, y, z));

			p_col[i] = y > 0.4f + 2.f * dy ? glm::vec3(0.0f, 1.0f, 0.0f) : y < 0.4f + dy ? glm::vec3(0.0f, 1.0f, 1.0f) : glm::vec3(1.0f, 0.0f, 1.0f); // color according to height
		}

		{ // add the color into the buffer for each particle
			glBindBuffer(GL_ARRAY_BUFFER, VBO_particles[1]);
			glBufferData(GL_ARRAY_BUFFER, utils::maxParticles * sizeof(glm::vec3), p_col, GL_STATIC_DRAW);
		}
		delete[] p_col;
	}

	n_particles = sim.dumpPositionsNormalized(p_pos);
	MSG(n_particles);
	utils::LastFrame = (float)glfwGetTime();

#ifdef WRITE_DATA_SBF
	// create writter
	if (!CreateDirectory("sim_files", NULL) && !ERROR_ALREADY_EXISTS == GetLastError())
	{
		MSG("ERROR::CANNOT CREATE DIRECTORY FOR FILES");
		return -1;
	}
	WriteSBF writter("sim_files/data.sbf", n_particles);
#endif

	while (!glfwWindowShouldClose(window) && !doSimulation) 
	{
		float currentFrame = utils::updateTime();

		processInput(window);

		draw(sim, p_pos);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	deactivateCallbacks(window);

	int iteration = -1;
	while (!glfwWindowShouldClose(window) && doSimulation)
	{
		if (iteration > 300) break;
		++iteration;

		float currentFrame = utils::updateTime();

		processInputLess(window);

		for(int i = 0; i < 200; ++i) sim.step(0.00006f);

		draw(sim, p_pos);

		std::cerr << "Draw " << 1.0f/utils::DeltaTime << std::endl;
		glBindVertexArray(0);

#ifdef PRINT_IMAGES_FLAG
		{
			char *data = new char[3u * utils::SCR_WIDTH * utils::SCR_HEIGHT];

			glReadPixels(0, 0, utils::SCR_WIDTH, utils::SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, data);

			utils::utilF::writeImageToDisk("ft_", iteration, utils::SCR_WIDTH, utils::SCR_HEIGHT, 3, data);
			delete[] data;
		}
#endif // PRINT_IMAGES_FLAG
#ifdef WRITE_DATA_SBO
		writter.writeData3f(p_pos);
#endif

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete[] p_pos;
	
	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	utils::SCR_WIDTH = width;
	utils::SCR_HEIGHT = height;

	// TODO: Observator system to update all projection matrix in shaders
	const glm::mat4 projection = glm::perspective(glm::radians(camera.m_zoom), static_cast<float>(utils::SCR_WIDTH) / utils::SCR_HEIGHT, 0.1f, 10.0f);
	const glm::mat4 projectionView = projection * camera.GetViewMatrix();

	shaderBB.use();
	shaderBB.setMat4("projectionView", projectionView);

}

void processInput(GLFWwindow* window)
{
	// Exit the simulation if ESCAPE
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	float d = GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) ? 2.0f : 1.0f;

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		doSimulation = true;

	float cameraSpeed = 2.5f * utils::DeltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, utils::DeltaTime * d);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, utils::DeltaTime * d);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, utils::DeltaTime * d);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, utils::DeltaTime * d);

}

void processInputLess(GLFWwindow* window)
{
	// Exit the simulation if ESCAPE
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float xposf = static_cast<float>(xpos);
	float yposf = static_cast<float>(ypos);
	
	if (firstMouse)
	{
		lastX = xposf;
		lastY = yposf;

		firstMouse = false;
	}

	float xoffset = xposf - lastX;
	float yoffset = yposf - lastY;

	lastX = xposf;
	lastY = yposf;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
