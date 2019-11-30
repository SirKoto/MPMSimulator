
#pragma once

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


int writeSimulation(Simulator_3D& sim, SimVisualizer* viewer, const int num_p, std::string fileName,
	const int framesToDo = 300, const float* colorData = nullptr);

int doSimulation();
int doConsoleSimulation();
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
	std::cout << " ]" << std::flush;
}
int create3BoxesFilledHomo(Simulator_3D& sim, glm::vec3*& p_col, int _n_particles = utils::maxParticles, const glm::vec3& velocity = glm::vec3(0.0f))
{
	int p_perBox = _n_particles / 3;
	int p_perDimension = static_cast<int>(std::floor(std::cbrt(p_perBox)));
	p_perBox = static_cast<int>(std::pow(p_perDimension, 3));
	int n_particles = p_perBox * 3;

	float dx = 0.2f / static_cast<float>(p_perDimension);

	p_col = new glm::vec3[n_particles];

	glm::vec3 inPos[] = { glm::vec3(0.4f), glm::vec3(0.6f), glm::vec3(0.2f) };
	glm::vec3 color[] = {glm::vec3(1,1,0), glm::vec3(1,0,1), glm::vec3(0,1,1) };
	for (int p = 0; p < 3; p++)
	{
		float x = inPos[p].x;

		for (int i = 0; i < p_perDimension; ++i, x += dx)
		{
			float y = inPos[p].y;

			for (int j = 0; j < p_perDimension; ++j, y += dx)
			{
				float z = inPos[p].z;

				for (int k = 0; k < p_perDimension; ++k, z += dx)
				{
					sim.addParticleNormalized(glm::vec3(x, y, z), velocity);
				}
			}
		}

		// fill color
		for (int i = p * p_perBox; i < (p + 1) * p_perBox; ++i)
		{
			p_col[i] = color[p];
		}
	}

	return n_particles;
} 

int createBoxFilledHomo(Simulator_3D& sim, glm::vec3*& p_col, int _n_particles = utils::maxParticles,
	float x0 = 0.1f, float x1 = 0.9f, float y0 = 0.65f, float y1 = 0.98f, float z0 = 0.3f, float z1 = 0.7f)
{
	const float vol = (x1 - x0) * (y1 - y0) * (z1 - z0);
	const float p = _n_particles * (1 / vol);
	const float pDim = std::cbrtf(p);
	const float dx = 1 / pDim;
	int n_particles = 0;
	for (float x =x0; x < x1; x += dx)
	{
		for (float y = y0; y < y1; y += dx)
		{
			for (float z = z0; z < z1; z += dx)
			{
				++n_particles;
			}
		}
	}
	p_col = new glm::vec3[n_particles];

	glm::vec3 color[] = { glm::vec3(1,1,0), glm::vec3(1,0,1), glm::vec3(0,1,1) };

	const float dy = (y1 - y0) / 3;
	int i = 0;
	for (float x = x0; x < x1; x += dx)
	{
		for (float y = y0; y < y1; y += dx)
		{
			for (float z = z0; z < z1; z += dx, i++)
			{
				sim.addParticleNormalized(glm::vec3(x, y, z));
				p_col[i] = color[y > y0 + 2.f * dy ? 0 : y > y0 + dy ? 1 : 2];
			}
		}
	}

	return n_particles;
}

int createBoxFilled(Simulator_3D &sim, glm::vec3* &p_col, int n_particles = utils::maxParticles)
{
	p_col = new glm::vec3[n_particles];

	// add random particles
	std::mt19937 mt_rng(42+1);
	std::uniform_real_distribution<float> disX(0.1f, 0.9f);
	std::uniform_real_distribution<float> disZ(0.3f, 0.7f);
	std::uniform_real_distribution<float> disY(0.4f, 0.8f);

	const float dy = (0.8f - 0.4f) / 3;
	for (int i = 0; i < n_particles; ++i)
	{
		float x = disX(mt_rng);
		float y = disY(mt_rng);
		float z = disZ(mt_rng);
		sim.addParticleNormalized(glm::vec3(x, y, z), glm::vec3(0.0f, 1.0f, -2.0f));

		p_col[i] = y > 0.4f + 2.f * dy ? glm::vec3(0.0f, 1.0f, 0.0f) : y < 0.4f + dy ? glm::vec3(0.0f, 1.0f, 1.0f) : glm::vec3(1.0f, 0.0f, 1.0f); // color according to height
	}

	return n_particles;
}

Simulator_3D loadSimulation(size_t &n_particles, glm::vec3* &p_col)
{
	float young, mu;
	std::cout << "Young Modulus(1e5): ";
	std::cin >> young;
	std::cout << "Mu(0.3): ";
	std::cin >> mu;
	MSG("Hyperelasticity");
	TMSG("1 - Corotated");
	TMSG("2 - Neo-Hookean");
	TMSG("3 - Sand");
	int n; std::cin >> n;

	Simulator_3D::HYPERELASTICITY hyper = Simulator_3D::HYPERELASTICITY::COROTATED;;
	switch (n)
	{
	case 1:
		hyper = Simulator_3D::HYPERELASTICITY::COROTATED;
		break;
	case 2:
		hyper = Simulator_3D::HYPERELASTICITY::NEOHOOKEAN;
		break;
	case 3:
		hyper = Simulator_3D::HYPERELASTICITY::SAND;
		break;
	default:
		break;
	}

	if(!young || !mu)
	{
		young = 1e5f;
		mu = 0.3f;
	}
	Simulator_3D sim(young, mu, hyper);

	int num = 10000;
	std::cout << "Number of particles: ";
	std::cin >> num;

	MSG("Model to load");
	TMSG("1 - Box Filled"); 
	TMSG("2 - Box Filled Homogen");
	TMSG("3 - 3 Boxes");
	std::cin >> n;
	switch (n)
	{
	case 1:
		n_particles = createBoxFilled(sim, p_col, num);
		break;

	case 2:
		n_particles = createBoxFilledHomo(sim, p_col, num);
		break;

	case 3:
		std::cout << "Initial velocity x y z: ";
		float x, y, z;
		std::cin >> x >> y >> z;
		n_particles = create3BoxesFilledHomo(sim, p_col, num, glm::vec3(x,y,z));
		break;

	default:
		n_particles = createBoxFilled(sim, p_col, num);
		break;
	}

	std::cout << "Add physics? (0/1): ";
	int ph; std::cin >> ph;
	if (ph)
	{
		MSG("Choose shape: ");
		TMSG("1 - Flat");
		TMSG("2 - Square Hourglasss");
		TMSG("3 - Two slopes");
		std::cin >> ph;
		switch (ph)
		{
		case 2:
			sim.setPhysicSlopes(0.25f, 0.4f, 0.1f, 2);
			sim.setPhysicsZWall(0.3f, 0.7f, 2);
			break;

		case 3:

			sim.setPhysicSlopes(0.25f, 0.4f, 0.1f, 2);
			break;

		case 1:
			std::cout << "Flat. Set height(0-1): ";
			float h; std::cin >> h;
			assert(h >= 0.0f && h <= 1.0f);
			sim.setPhysicsFlat(h);
		default:
			break;
		}
	}
	return sim;
}

int doConsoleSimulation()
{
	glm::vec3* p_col = nullptr;
	size_t n_particles;
	Simulator_3D sim = loadSimulation(n_particles, p_col);
	assert(p_col != nullptr);

	float* colordata = new float[3 * n_particles];
	std::memcpy(colordata, p_col, 3 * n_particles * sizeof(float));
	delete[] p_col;

	std::string fileName;
	MSG("Enter filename:");
	std::cin >> fileName;

	int n_frames;
	MSG("Enter number of frames:");
	std::cin >> n_frames;

	return writeSimulation(sim, nullptr, static_cast<int>(n_particles), fileName, n_frames, colordata);

}


int doSimulation()
{
	// Create simulator and add points
	glm::vec3* p_col = nullptr;
	size_t n_particles;
	Simulator_3D sim = loadSimulation(n_particles, p_col);

	// always create color
	assert(p_col != nullptr);

	SimVisualizer viewer(static_cast<int>(n_particles), false);
	if (viewer.ErrorHappened())
	{
		MSG("ERROR on SimVisualizer creation");
		return -1;
	}
	float* p_pos = new float[3 * n_particles];

	float* colordata = new float[3 * n_particles];
	{
		std::memcpy(colordata, p_col, 3 * n_particles * sizeof(float));
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

		int n_frames;
		MSG("Enter number of frames:");
		std::cin >> n_frames;

		return writeSimulation(sim, &viewer, static_cast<int>(n_particles), fileName, n_frames, colordata);
	}
	return 0;
}

int writeSimulation(Simulator_3D& sim, SimVisualizer* const viewer, const int num_p, std::string fileName,
	const int framesToDo, const float* colorData)
{
	// create directory if it does not exist
	if (!utils::utilF::createDir("sim_files"))
	{
		std::cout << "ERROR::CANNOT CREATE DIR" << std::endl;
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
	else
		MSG("WARNING::WRITER:NO COLOR ASSIGNED");

	float* p_pos = new float[3 * static_cast<size_t>(num_p)];


	if(viewer) viewer->enableUserInput(false);

	constexpr float step_t = 1e-5f;
	constexpr float secondsPerFrame = 1 / 60.0f;
	constexpr int simPerFrame = static_cast<int>(secondsPerFrame / step_t);

	writer.writeDataf(step_t, SBF_DT_FRAMES);
	writer.writeDataf(sim.getYoung(), SBF_PARAM_E);
	writer.writeDataf(sim.getNu(), SBF_PARAM_NU);
	sim.dumpPositionsNormalized(p_pos);
	writer.writeData3f(p_pos, SBF_DATA);

	int frame = 0;
	while (!viewer || !viewer->shouldApplicationClose())
	{
		// do n frames
		if (frame++ > framesToDo) break;

		auto start = std::chrono::high_resolution_clock::now();

		std::cout << std::endl;
		for (int i = 0; i < simPerFrame; ++i) 
		{ 
			sim.step(step_t); 

			if (viewer)
			{
				viewer->temptateEvents();
				if (viewer->shouldApplicationClose())
					break;
			}

			if (i % 10 == 0)
			{
				float percent = i / static_cast<float>(simPerFrame);
				printProgress(percent);

				if (viewer)
				{
					sim.dumpPositionsNormalized(p_pos);
					viewer->updateParticlePositions(p_pos);
					viewer->draw();
				}
			}
		}

		auto end = std::chrono::high_resolution_clock::now();

		sim.dumpPositionsNormalized(p_pos);
		if (viewer)
		{
			viewer->updateParticlePositions(p_pos);

			viewer->draw();
		}
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
	while (res != SBF_ERROR)
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
		std::function<void()> reset = [&i]() {i = 0; };

		viewer.setKeyCallback(SimVisualizer::KEYS::K, inc);
		viewer.setKeyCallback(SimVisualizer::KEYS::L, dec);
		viewer.setKeyCallback(SimVisualizer::KEYS::O, reset);

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
		
		std::function<void()> writeGIf = [&frames, &viewer, &milisPerFrame]() {
			std::cout << "Enter file name to gif (0 to cancel): ";
			std::string filename;

			viewer.enableUserInput(false);
			std::cin >> filename;

			if (filename != "0")
				utils::utilF::writeFramesAsGif(frames, viewer, milisPerFrame, filename);

			viewer.enableUserInput(true);
		};
		viewer.setKeyCallback(SimVisualizer::KEYS::I, writeGIf);

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
		if (frames.empty()) MSG("NO-FRAMES-TO-SEE");
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
		TMSG("2 - Do console simulation");
		TMSG("3 - Read simulation");
		TMSG("4 - Exit");
	} while (std::cin >> res && (res < 1 || res > 4));


	switch (res)
	{
	case 1:
		res = doSimulation();
		break;

	case 2:
		res = doConsoleSimulation();
		break;

	case 3:
		res = readSimulation();
		break;

	case 4:

	default:
		break;
	}

	return res;
}
