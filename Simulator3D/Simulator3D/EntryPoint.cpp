
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


int writeSimulation(Simulator_3D& sim, SimVisualizer& viewer, const int num_p, std::string fileName,
	const int framesToDo = 300, const float* colorData = nullptr);

int doSimulation();

int readSimulation();

void printProgress(float p)
{
	float p5 = p * (100 / 5); // 5%
	std::cout << "\r[ ";
	for (int i = 0; i < (100 / 5); ++i)
	{
		if (p5 >= i)
		{
			std::cout << "#";
		}
		else
		{
			std::cout << " ";
		}
	}
	std::cout << " ]";
}

int doSimulation()
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
	Simulator_3D sim(1e5f, 0.3f);
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

	float* colordata = new float[3 * utils::maxParticles];
	{
		std::memcpy(colordata, p_col, 3 * utils::maxParticles * sizeof(float));
		viewer.updateParticlesColor(colordata);
	}
	delete[] p_col;

	n_particles = sim.dumpPositionsNormalized(p_pos);
	MSG(n_particles);

	viewer.updateParticlePositions(p_pos);

	delete[] p_pos;

	bool enterPressed = false;
	{
		std::function<void()> f = [&enterPressed]() {enterPressed = true; };
		viewer.setKeyCallback(SimVisualizer::KEYS::ENTER, f);
	}

	// show simulation stopped meanwhile
	while (!viewer.shouldApplicationClose() && !enterPressed)
	{
		viewer.draw();
	}


	if (enterPressed)
	{
		//disable user input
		viewer.enableUserInput(false);

		std::string fileName;
		MSG("Enter filename");
		std::cin >> fileName;

		return writeSimulation(sim, viewer, n_particles, fileName, 100, colordata);
	}
	return 0;
}

int writeSimulation(Simulator_3D& sim, SimVisualizer& viewer, const int num_p, std::string fileName,
	const int framesToDo, const float* colorData)
{
	// create directory if it does not exist
	if (!CreateDirectory("sim_files", NULL) && !ERROR_ALREADY_EXISTS == GetLastError())
	{
		MSG("ERROR::SBF::CANNOT CREATE DIRECTORY FOR FILES");
		return -1;
	}

	// add sbf to filename if it does not have it
	if (fileName.size() < 6 || fileName.compare(fileName.size() - 4, 4, ".sbf") != 0)
	{
		fileName.append(".sbf");
	}

	MSG("writing on " << fileName);
	// create writer
	WriteSBF writer("sim_files/" + fileName, num_p);

	if (!writer.canWrite())
	{
		MSG("ERROR::WRITER::CANNOT WRITE");
		return -1;
	}
	if (colorData != nullptr)
	{
		writer.writeData3f(colorData, SBF_COLOR);
	}

	float* p_pos = new float[3 * static_cast<size_t>(num_p)];


	viewer.enableUserInput(false);

	constexpr float step_t = 0.00003f;
	constexpr float secondsPerFrame = 1 / 60.0f;
	constexpr int simPerFrame = static_cast<int>(secondsPerFrame / step_t);

	writer.writeDataf(step_t, SBF_DT_FRAMES);
	writer.writeDataf(sim.getYoung(), SBF_PARAM_E);
	writer.writeDataf(sim.getNu(), SBF_PARAM_NU);

	int frame = 0;
	while (!viewer.shouldApplicationClose())
	{
		// do n frames
		if (frame++ > framesToDo) break;

		auto start = std::chrono::high_resolution_clock::now();

		std::cout << std::endl;
		for (int i = 0; i < simPerFrame; ++i) 
		{ 
			sim.step(step_t); 

			float percent = i / static_cast<float>(simPerFrame);
			printProgress(percent);


			viewer.temptateEvents();
			if (viewer.shouldApplicationClose())
				break;
		}

		auto end = std::chrono::high_resolution_clock::now();

		sim.dumpPositionsNormalized(p_pos);
		viewer.updateParticlePositions(p_pos);

		viewer.draw();

		// write data into file
		writer.writeData3f(p_pos, SBF_DATA);

		MSG(" " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " s, Frame " << frame);

#ifdef PRINT_IMAGES_FLAG
		{
			char* data = new char[3u * utils::SCR_WIDTH * utils::SCR_HEIGHT];

			glReadPixels(0, 0, utils::SCR_WIDTH, utils::SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, data);

			utils::utilF::writeImageToDisk("ft_", iteration, utils::SCR_WIDTH, utils::SCR_HEIGHT, 3, data);
			delete[] data;
		}
#endif // PRINT_IMAGES_FLAG

	}

	return 0;
}

int readSimulation()
{
	std::string fileName;
	MSG("Enter filename");
	std::cin >> fileName;

	// add sbf to filename if it does not have it
	if (fileName.size() < 6 || fileName.compare(fileName.size() - 4, 4, ".sbf") != 0)
	{
		fileName.append(".sbf");
	}

	// create writer
	ReadSBF reader("sim_files/" + fileName);
	if (!reader.canRead())
	{
		MSG("ERROR::READER::CANNOT READ");
		return -1;
	}

	const int n_particles = reader.GetNumberParticles();

	float* color = new float[3 * static_cast<size_t>(n_particles)];

	std::vector<FrameSBF<float>> frames(0); // by default 300 frames

	char res = reader.ReadNextFlag(false);
	while (res != SBF_EOF && res != SBF_ERROR)
	{
		switch (res)
		{
		case SBF_COLOR:
			reader.ReadData3f(color);
			break;

		case SBF_DATA:
			// put the new frame at the back (constructed)
			frames.emplace_back();
			// Reserve memory
			frames.back().prepareData(3 * n_particles);
			// dump data
			reader.ReadData3f(frames.back().ptr());
			break;

		case SBF_DT_FRAMES:
			MSG("Dt: " << static_cast<float>(reader.ReadDataf()) << " s");
			break;

		case SBF_PARAM_E:
			MSG("Younk modulus: " << static_cast<float>(reader.ReadDataf()));
			break;

		default:
			break;
		}
		res = reader.ReadNextFlag(false);
	}

	// show simulation
	SimVisualizer viewer(n_particles);
	if (!viewer.ErrorHappened() && !frames.empty())
	{
		viewer.enableUserInput(true);
		viewer.updateParticlesColor(color);

		// set particle positions for frame 0
		viewer.updateParticlePositions(frames[0]);
		int i = -1;

		int milisPerFrame = 30;
		std::function<void()> inc = [&milisPerFrame]() {if(milisPerFrame != 0) milisPerFrame++; };
		std::function<void()> dec = [&milisPerFrame]() {if(milisPerFrame >= 2) milisPerFrame--; };
		viewer.setKeyCallback(SimVisualizer::KEYS::K, inc);
		viewer.setKeyCallback(SimVisualizer::KEYS::L, dec);
		int buff = 0;
		std::function<void()> pause = [&milisPerFrame, &buff]() {
			if (buff == 0)
				std::swap(buff, milisPerFrame);
		};
		std::function<void()> unpause = [&milisPerFrame, &buff]() {
			if (milisPerFrame == 0)
				std::swap(buff, milisPerFrame);
		};
		viewer.setKeyCallback(SimVisualizer::KEYS::ENTER, unpause);
		viewer.setKeyCallback(SimVisualizer::KEYS::P, pause);

		while (!viewer.shouldApplicationClose())
		{
			auto last = std::chrono::high_resolution_clock::now();
			auto count = std::chrono::milliseconds(milisPerFrame);
			if (milisPerFrame != 0 && ++i >= frames.size())
				i = 0;
			viewer.updateParticlePositions(frames[i]);
			viewer.draw();

			while (count > std::chrono::high_resolution_clock::duration::zero())
			{
				auto now = std::chrono::high_resolution_clock::now();
				auto dt = now - last;
				count -= std::chrono::duration_cast<std::chrono::milliseconds>(dt);
				last = now;
				viewer.draw();
			}
		}

		res = 0;
	}
	else
	{
		MSG("ERROR::VIEWER");
		res = -1;
	}

	// delete all data
	const int size = static_cast<int>(frames.size());
	for (int i = 0; i < size; ++i)
	{
		frames[i].deleteData();
	}
	delete[] color;

	return res;
}

int main()
{
	int res;
	do{
		MSG("Do you want to do a new simulation or read a sbf file?");
		TMSG("1 - Do simulation");
		TMSG("2 - Read simulation");
		TMSG("3 - Exit");
	} while (std::cin >> res && (res < 1 || res > 3));


	switch (res)
	{
	case 1:
		res = doSimulation();
		break;

	case 2:
		res = readSimulation();
		break;

	case 3:

	default:
		break;
	}

	return res;
}