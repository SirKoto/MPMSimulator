
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
#include "Simulator_2D.h"


//#define PRINT_IMAGES_FLAG

struct ParticlePos
{
	float x, y;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


Shader shader;

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
	//glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);
	//Camputa mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

int main()
{
	GLFWwindow* window = NULL;
	if (!initGLFW(window))
		return -1;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Clear color state

	setCallbacks(window);

	float* p_pos = new float[utils::maxParticles * 2];

	GLuint VAO, VBO;
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, utils::maxParticles * 2 * sizeof(float), p_pos, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);
		glEnableVertexAttribArray(0);
	}

	//const glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(utils::SCR_WIDTH), 0.0f, static_cast<float>(utils::SCR_HEIGHT));
	const glm::mat4 projection = glm::ortho(-5.0f, 85.0f, -5.0f, 85.0f);
	shader =  Shader("shaders/shaderPoint.vert", "shaders/shaderPoint.frag");
	shader.use();
	shader.setMat4("projection", projection);

	glPointSize(4.0f); // Drawing points

	int n_particles = 9000;

	// Create simulator and add points
	Simulator_2D sim;
	{
		// add random particles

		std::mt19937 mt_rng(42);
		std::uniform_real_distribution<float> dis(-10.0f, 8.0f);
		std::uniform_real_distribution<float> up_v(0, 2.0f);


		for (int i = 0; i < n_particles; ++i)
		{
			sim.addParticle(glm::vec2(dis(mt_rng), dis(mt_rng)) + glm::vec2(40.0f,50.0f), glm::vec2(up_v(mt_rng) - 1.0f, up_v(mt_rng)));
		}
	}

	n_particles = sim.dumpPositions(p_pos);
	std::cerr << n_particles << std::endl;

	utils::LastFrame = (float)glfwGetTime();
	sim.step(0.2f);
	int iteration = -1;
	while (!glfwWindowShouldClose(window))
	{
		++iteration;

		float currentFrame = (float)glfwGetTime();
		utils::DeltaTime = currentFrame - utils::LastFrame;
		utils::LastFrame = currentFrame;

		processInput(window);


		for(int i = 0; i < 3; ++i) sim.step(0.2f);

		n_particles = sim.dumpPositions(p_pos);

		/*for (int i = 0; i < 2 * n_particles; i += 2 )
		{
			p_pos[i] = p_pos[i] * utils::SCR_WIDTH ;
			p_pos[i + 1] = p_pos[i + 1] * utils::SCR_HEIGHT * sim.getAspectRatio();

		}*/


		glClear(GL_COLOR_BUFFER_BIT);


		// Draw
		glBindVertexArray(VAO);

		// update positions
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
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
	//const glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(utils::SCR_WIDTH), 0.0f, static_cast<float>(utils::SCR_HEIGHT));
	const glm::mat4 projection = glm::ortho(-5.0f, 85.0f, -5.0f, 85.0f);

	shader.use();
	shader.setMat4("projection", projection);

}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}