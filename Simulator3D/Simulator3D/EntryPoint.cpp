
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


// #define PRINT_IMAGES_FLAG

struct ParticlePos
{
	float x, y;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void processInputLess(GLFWwindow* window);


Shader shader, shaderBB;
Camera camera(glm::vec3(0.5f, 0.0f, 10.0f));

bool firstMouse = true;
bool doSimulation = false;
float lastX, lastY;

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

void initArraysParticles(GLuint& VAO, GLuint* VBO, float* &positions, glm::vec3* &colors)
{
	positions = new float[utils::maxParticles * 3];
	colors = new glm::vec3[utils::maxParticles];

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(2, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, utils::maxParticles * 3 * sizeof(float), positions, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, utils::maxParticles * sizeof(glm::vec3), colors, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	shader = Shader("shaders/shaderPoint.vert", "shaders/shaderPoint.frag");

	glPointSize(10.0f); // Drawing points
}



void initArraysBB(GLuint& VAO, GLuint* VBO)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(2, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(utils::CubeVertices), utils::CubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(utils::CubeIndices), utils::CubeIndices, GL_STATIC_DRAW);

	shaderBB = Shader("shaders/shaderBB.vert", "shaders/shaderPoint.frag");
	const glm::vec3 colorBB = glm::vec3(1.0f, 0.0, 0.0f);

	shaderBB.use();
	shaderBB.setVec3("colorBBox", colorBB);

}

void drawBB(const GLuint VAO, const GLuint* VBO)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	shaderBB.use();

	const glm::mat4 projection = glm::perspective(glm::radians(camera.m_zoom), static_cast<float>(utils::SCR_WIDTH) / utils::SCR_HEIGHT, 0.1f, 10.0f);
	const glm::mat4 projectionView = projection * camera.GetViewMatrix();
	shaderBB.setMat4("projectionView", projectionView);
	const glm::vec3 colorBB = glm::vec3(1.0f, 0.0, 0.0f);
	shaderBB.setVec3("colorBBox", colorBB);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
	glDrawElements(GL_TRIANGLES, sizeof(utils::CubeIndices), GL_UNSIGNED_BYTE, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void drawParticles(const GLuint VAO, const GLuint* VBO, const Simulator_3D& sim, float* particleDump)
{
	unsigned int n = sim.dumpPositionsNormalized(particleDump);

	// Draw
	glBindVertexArray(VAO);

	// update positions
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * (3 * sizeof(float)), particleDump);

	shader.use();
	const glm::mat4 projection = glm::perspective(glm::radians(camera.m_zoom), static_cast<float>(utils::SCR_WIDTH) / utils::SCR_HEIGHT, 0.1f, 10.0f);
	const glm::mat4 projectionView = projection * camera.GetViewMatrix();
	shader.setMat4("projectionView", projectionView);

	glDrawArrays(GL_POINTS, 0, n);
}

int main()
{
	GLFWwindow* window = NULL;
	if (!initGLFW(window))
		return -1;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Clear color state

	setCallbacks(window);

	float* p_pos;
	glm::vec3* p_col;
	// VBO[0] := positions, VBO[1] := color
	GLuint VAO_particles, VBO_particles[2];
	initArraysParticles(VAO_particles, VBO_particles, p_pos, p_col);

	GLuint VAO_BB, VBO_BB[2];
	initArraysBB(VAO_BB, VBO_BB);

	

	int n_particles = utils::maxParticles;

	// Create simulator and add points
	Simulator_3D sim;
	{
		// add random particles

		std::mt19937 mt_rng(42);
		std::uniform_real_distribution<float> dis(0.4f, 0.6f);


		for (int i = 0; i < n_particles; ++i)
		{
			float x = dis(mt_rng);
			float y = dis(mt_rng);
			float z = dis(mt_rng);
			sim.addParticleNormalized(glm::vec3(x, y, z));

			p_col[i] = y > 8.0f ? glm::vec3(0.0f, 1.0f, 0.0f) : y < -8.0f ? glm::vec3(0.0f,1.0f,1.0f): glm::vec3(1.0f, 0.0f, 1.0f); // color according to height
		}

		{ // add the color into the buffer for each particle
			glBindBuffer(GL_ARRAY_BUFFER, VBO_particles[1]);
			glBufferData(GL_ARRAY_BUFFER, utils::maxParticles * sizeof(glm::vec3), p_col, GL_STATIC_DRAW);
		}
	}

	n_particles = sim.dumpPositionsNormalized(p_pos);
	MSG(n_particles);
	utils::LastFrame = (float)glfwGetTime();

	while (!glfwWindowShouldClose(window) && !doSimulation) 
	{
		float currentFrame = utils::updateTime();

		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT);

		drawBB(VAO_BB, VBO_BB);
		drawParticles(VAO_particles, VBO_particles, sim, p_pos);
		//MSG(camera.m_position.x << " " << camera.m_position.y << " " << camera.m_position.z << ", " << camera.m_front.x << " " << camera.m_front.y << " " << camera.m_front.z);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	int iteration = -1;
	while (!glfwWindowShouldClose(window) && doSimulation)
	{
		++iteration;

		float currentFrame = utils::updateTime();

		processInputLess(window);

		for(int i = 0; i < 5; ++i) sim.step(0.002f);


		drawParticles(VAO_particles, VBO_particles, sim, p_pos);

		glClear(GL_COLOR_BUFFER_BIT);
		std::cerr << "Draw " << 1.0f/utils::DeltaTime << std::endl;
		glBindVertexArray(0);

#ifdef PRINT_IMAGES_FLAG
		{
			char *data = new char[3 * utils::SCR_WIDTH * utils::SCR_HEIGHT];

			glReadPixels(0, 0, utils::SCR_WIDTH, utils::SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, data);

			utils::utilF::writeImageToDisk("ft_", iteration, utils::SCR_WIDTH, utils::SCR_HEIGHT, 3, data);
			delete[] data;
		}
#endif // PRINT_IMAGES_FLAG


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete[] p_pos;
	delete[] p_col;
	
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
