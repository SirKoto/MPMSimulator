#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <random>
#include "Utils.h"
#include "Shader.h"
#include "Simulator_2D.h"

struct ParticlePos
{
	float x, y;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

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

	float p_pos[500 * 2];

	GLuint VAO, VBO;
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, utils::maxParticles * 2 * sizeof(float), p_pos, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	const glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(utils::SCR_WIDTH), 0.0f, static_cast<float>(utils::SCR_HEIGHT));
	Shader shader("shaders/shaderPoint.vert", "shaders/shaderPoint.frag");
	shader.use();
	shader.setMat4("projection", projection);

	glPointSize(1.5f); // Drawing points

	int n_particles = 500;

	// Create simulator and add points
	Simulator_2D sim;
	{
		// add random particles

		std::mt19937 mt_rng;
		std::uniform_real_distribution<float> dis(-5.0f, 5.0f);

		for (int i = 0; i < n_particles; ++i)
		{
			sim.addParticle(glm::vec2(dis(mt_rng), dis(mt_rng)) + glm::vec2(40.0f,50.0f));
		}
	}
	utils::LastFrame = (float)glfwGetTime();
	n_particles = sim.dumpPositions(p_pos);
	std::cerr << n_particles << std::endl;
	sim.step(1e-4f);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		utils::DeltaTime = currentFrame - utils::LastFrame;
		utils::LastFrame = currentFrame;

		processInput(window);


		for(int i = 0; i < 1; ++i) sim.step(1e-3f);

		n_particles = sim.dumpPositions(p_pos);

		for (int i = 0; i < 2 * n_particles; i += 1)
		{
			p_pos[i] *= 5.0f;
		}


		glClear(GL_COLOR_BUFFER_BIT);


		// Draw
		glBindVertexArray(VAO);

		// update positions
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, n_particles * (2 * sizeof(float)), p_pos);
		shader.use();
		std::cerr << "Draw" << std::endl;
		glDrawArrays(GL_POINTS, 0, n_particles);


		glBindVertexArray(0);


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
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}