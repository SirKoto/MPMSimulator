
#pragma once

#include <Windows.h>

#include "Utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <random>

#include <thread>
#include <chrono>

#include "IO/WriteSBF.h"
#include "IO/ReadSBF.h"

#include "Simulator_3D.h"
#include "SimVisualizer.h"

// #define PRINT_IMAGES_FLAG
// #define WRITE_DATA_SBF
#define SHADOWS


int main()
{
	int n_particles = utils::maxParticles;
	SimVisualizer viewer(n_particles, false);
	if (viewer.ErrorHappened())
	{
		MSG("ERROR on SimVisualizer creation");
		return -1;
	}
	float* p_pos = new float[utils::maxParticles * 3];
	glm::vec3* p_col = new glm::vec3[utils::maxParticles];
	
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
	}

	{
		float* colordata = new float[3 * utils::maxParticles];
		std::memcpy(colordata, p_col, 3 * utils::maxParticles * sizeof(float));
		viewer.updateParticlesColor(colordata);
		delete[] colordata;
	}
	delete[] p_col;

	n_particles = sim.dumpPositionsNormalized(p_pos);
	MSG(n_particles);

	viewer.updateParticlePositions(p_pos);
#ifdef WRITE_DATA_SBF
	// create writter
	if (!CreateDirectory("sim_files", NULL) && !ERROR_ALREADY_EXISTS == GetLastError())
	{
		MSG("ERROR::SBF::CANNOT CREATE DIRECTORY FOR FILES");
		return -1;
	}
	WriteSBF writter("sim_files/data.sbf", n_particles);
#endif

	bool enterPressed = false;
	{
		std::function<void()> f = [&enterPressed]() {enterPressed = true; };
		viewer.setKeyCallback(SimVisualizer::KEYS::ENTER, f);
	}

	while (!viewer.shouldApplicationClose() && ! enterPressed) 
	{
		viewer.draw();
	}

	delete[] p_pos;
	/*

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
#ifdef WRITE_DATA_SBF
		writter.writeData3f(p_pos);
#endif

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete[] p_pos;
	
	glfwTerminate();
	*/
	return 0;
}