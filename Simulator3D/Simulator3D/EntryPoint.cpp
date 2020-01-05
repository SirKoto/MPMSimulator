
#pragma once

#include "Utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <chrono>

#include "IO/WriteSBF.h"
#include "IO/ReadSBF.h"

#include "Simulator_3D.h"
#include "SimVisualizer.h"

#include "ParticleStructures.h"

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

Simulator_3D loadSimulation(size_t &n_particles, glm::vec3* &p_col)
{
	
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

	
	Simulator_3D sim(hyper);

	float young, nu, hardening, volume, mass;
	int another = 0;
	do
	{
		std::cout << "Young Modulus(1e5): ";
		std::cin >> young;
		std::cout << "Nu(0.3): ";
		std::cin >> nu;
		std::cout << "hardening(10.0): ";
		std::cin >> hardening;
		std::cout << "volume(1.0): ";
		std::cin >> volume;
		std::cout << "mass(1.0): ";
		std::cin >> mass;
		if (!young)
		{
			young = 1e5f;
		}
		if (!nu)
		{
			nu = 0.3f;
		}
		if (!hardening)
		{
			hardening = 10.f;
		}
		if (!volume)
		{
			volume = 1.0f;
		}
		if (!mass)
		{
			mass = 1.0f;
		}

		sim.addNewMaterial(young, nu, hardening, volume, mass);
		std::cout << "Another material? (0/1): ";
		std::cin >> another;
	} while (another);



	int num = 10000;
	std::cout << "Number of particles: ";
	std::cin >> num;

	MSG("Model to load");
	TMSG("1 - Box Filled"); 
	TMSG("2 - Box Filled Homogen");
	TMSG("3 - 3 Boxes different heights");
	TMSG("4 - 3 Boxes aligned");
	TMSG("5 - Sphere coliding");
	std::cin >> n;
	switch (n)
	{
		float x, y, z;

	case 1:
		n_particles = ps::createBoxFilled(sim, p_col, num);
		break;

	case 2:
		n_particles = ps::createBoxFilledHomo(sim, p_col, num);
		break;

	case 3:
		std::cout << "Initial velocity x y z: ";
		std::cin >> x >> y >> z;
		n_particles = ps::create3BoxesFilledHomo(sim, p_col, num, glm::vec3(x, y, z));
		break;

	case 4:
		std::cout << "Initial velocity x y z: ";
		std::cin >> x >> y >> z;
		{
			std::cout << "Enter 3 material id: ";
			int tmp[3];
			std::cin >> tmp[0] >> tmp[1] >> tmp[2];
			n_particles = ps::create3BoxesSeparatedFilledHomo(sim, p_col, num, glm::vec3(x, y, z), tmp);
		}
		break;

	case 5:
		{
			std::cout << "Initial velocity: ";
			std::cin >> x;
			std::cout << "Enter radius: ";
			float r; std::cin >> r;
			int tmp[2];
			std::cout << "Enter 2 material id: ";
			std::cin >> tmp[0] >> tmp[1];
			n_particles = ps::create2CollidingSpheres(sim, p_col, num, r, x, tmp);
		}
		break;

	default:
		n_particles = ps::createBoxFilled(sim, p_col, num);
		break;
	}

	std::cout << "Add physics? (0/1): ";
	int ph; std::cin >> ph;
	if (ph)
	{
		MSG("Choose shape: ");
		TMSG("1 - Flat");
		TMSG("2 - Two slopes");
		TMSG("3 - Two slopes unbounded");
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
	int res = writeSimulation(sim, nullptr, static_cast<int>(n_particles), fileName, n_frames, colordata);
	delete[] colordata;
	return res;
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
		
		int res = writeSimulation(sim, &viewer, static_cast<int>(n_particles), fileName, n_frames, colordata);
		delete[] colordata;
		return res;
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

	std::cout << "FPS: "; 
	float fps; std::cin >> fps;
	const float step_t = 1e-5f;
	const float secondsPerFrame = 1.0f / (fps > 0 ? fps : 60.0f);
	const int simPerFrame = static_cast<int>(secondsPerFrame / step_t);
	MSG("writing on " << fileName);

	writer.writeDataf(step_t, SBF_DT_FRAMES);

	for (int i = 0; i < sim.getNumMaterials(); ++i)
	{
		writer.writeDataf(static_cast<float>(i), SBF_ID);
		writer.writeDataf(sim.getYoung(i), SBF_PARAM_E);
		writer.writeDataf(sim.getNu(i), SBF_PARAM_NU);
		writer.writeDataf(sim.getHardening(i), SBF_PARAM_HARDENING);
		writer.writeDataf(sim.getVolume(i), SBF_PARAM_VOLUME);
		writer.writeDataf(sim.getMass(i), SBF_PARAM_MASS);
	}

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

			if (i % 50 == 0)
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
	delete[] p_pos;
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
			TMSG("Young modulus: " << static_cast<float>(reader.ReadDataf()));
			break;

		case SBF_PARAM_NU:
			TMSG("Nu: " << static_cast<float>(reader.ReadDataf()));
			break;

		case SBF_PARAM_HARDENING:
			TMSG("Hardening: " << static_cast<float>(reader.ReadDataf()));
			break;

		case SBF_PARAM_VOLUME:
			TMSG("Volume: " << static_cast<float>(reader.ReadDataf()));
			break;

		case SBF_PARAM_MASS:
			TMSG("Mass: " << static_cast<float>(reader.ReadDataf()));
			break;

		case SBF_ID:
			MSG("Material: " << static_cast<float>(reader.ReadDataf()));
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
