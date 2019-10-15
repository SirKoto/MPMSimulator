
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


Shader shader;
Camera camera(glm::vec3(0.0f, 0.0f, -3.0f));

bool firstMouse = true;
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

int main()
{
	GLFWwindow* window = NULL;
	if (!initGLFW(window))
		return -1;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Clear color state

	setCallbacks(window);

	float* p_pos = new float[utils::maxParticles * 2];
	glm::vec3* p_col = new glm::vec3[utils::maxParticles];
	GLuint VAO, VBO[2];
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(2, VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, utils::maxParticles * 2 * sizeof(float), p_pos, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, utils::maxParticles * sizeof(glm::vec3), p_col, GL_STATIC_DRAW);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
		glEnableVertexAttribArray(1);
	}

	const glm::mat4 projection = glm::perspective(glm::radians(camera.m_zoom), static_cast<float>(utils::SCR_WIDTH) / utils::SCR_HEIGHT, 0.1f, 200.0f);

	shader =  Shader("shaders/shaderPoint.vert", "shaders/shaderPoint.frag");
	shader.use();
	shader.setMat4("projection", projection);

	glPointSize(4.0f); // Drawing points

	int n_particles = 1000;

	// Create simulator and add points
	Simulator_2D sim;
	{
		// add random particles

		std::mt19937 mt_rng(42);
		std::uniform_real_distribution<float> dis(-20.0f, 20.0f);
		std::uniform_real_distribution<float> up_v(0, 2.0f);


		for (int i = 0; i < n_particles; ++i)
		{
			float height = dis(mt_rng);
			sim.addParticle(glm::vec2(dis(mt_rng), height) + glm::vec2(40.0f,50.0f), 0.0f * glm::vec2(up_v(mt_rng) - 1.0f, up_v(mt_rng)));
			p_col[i] = height > 8.0f ? glm::vec3(0.0f, 1.0f, 0.0f) : height < -8.0f ? glm::vec3(0.0f,1.0f,1.0f): glm::vec3(1.0f, 0.0f, 1.0f); // color according to height
		}

		{ // add the color into the buffer for each particle
			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			glBufferData(GL_ARRAY_BUFFER, utils::maxParticles * sizeof(glm::vec3), p_col, GL_STATIC_DRAW);
		}
	}

	n_particles = sim.dumpPositions(p_pos);
	MSG(n_particles);

	utils::LastFrame = (float)glfwGetTime();
	sim.step(0.002f);
	int iteration = -1;
	while (!glfwWindowShouldClose(window))
	{
		++iteration;

		float currentFrame = (float)glfwGetTime();
		utils::DeltaTime = currentFrame - utils::LastFrame;
		utils::LastFrame = currentFrame;

		processInput(window);


		for(int i = 0; i < 5; ++i) sim.step(0.002f);

		n_particles = sim.dumpPositions(p_pos);


		glClear(GL_COLOR_BUFFER_BIT);


		// Draw
		glBindVertexArray(VAO);

		// update positions
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, n_particles * (2 * sizeof(float)), p_pos);
		shader.use();
		std::cerr << "Draw " << 1.0f/utils::DeltaTime << std::endl;
		glDrawArrays(GL_POINTS, 0, n_particles);


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
	
	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	utils::SCR_WIDTH = width;
	utils::SCR_HEIGHT = height;

	// TODO: Observator system to update all projection matrix in shaders
	const glm::mat4 projection = glm::perspective(glm::radians(camera.m_zoom), static_cast<float>(utils::SCR_WIDTH) / utils::SCR_HEIGHT, 0.1f, 200.0f);

	shader.use();
	shader.setMat4("projection", projection);

}

void processInput(GLFWwindow* window)
{
	// Exit the simulation if ESCAPE
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	float d = GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) ? 2.0f : 1.0f;

	float cameraSpeed = 2.5f * utils::DeltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, utils::DeltaTime * d);
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}