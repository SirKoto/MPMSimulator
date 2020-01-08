#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <Eigen/Dense>

#include <iostream>
#include <string>

#include "SimVisualizer.h"
#include "IO/FrameSBF.h"

// Utils methods

#define MSG(m) std::cout << m << std::endl 
#define TMSG(m) std::cout << "\t" << m << std::endl

 
const char SBF_DATA = (0xFFu);
const char SBF_EOF = (0x00u);
const char SBF_COLOR = (0x01u);
const char SBF_PARAM_E = (0x02u);
const char SBF_PARAM_NU = (0x03u);
const char SBF_DT_FRAMES = (0x04u);
const char SBF_PARAM_HARDENING = (0x05u);
const char SBF_PARAM_VOLUME = (0x06u);
const char SBF_PARAM_MASS = (0x07u);
const char SBF_ID = (0x08u);
const char SBF_PLASTIC = (0x09u);
const char SBF_T_C = (0x0Au);
const char SBF_T_S = (0x0Bu);
const char SBF_MODEL = (0x0Cu);
const char SBF_ERROR = (0xFEu);



namespace utils
{
	// Basic screen size
	static unsigned int SCR_WIDTH = 800;
	static unsigned int SCR_HEIGHT = 600;

	// Shadow texture size
	constexpr unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	// Default number of particles, just in case
	constexpr unsigned int maxParticles = 60000;

	// DEPICATED POLAR DECOMPOSITIONS
	inline void polarDecomposition2D(const Eigen::Matrix2f& m, Eigen::Matrix2f& s, Eigen::Matrix2f& r)
	{
		float x = m(1,1) + m(0,0); // trace
		float y = m(1,0) - m(0,1);
		float scale = 1.0f / std::sqrt(x * x + y * y);
		float c = x * scale;
		float d = y * scale;
		r << c, -d,
			d, c;
		/*
		r[1][1] = c;
		r[0][0] = c;
		r[0][1] = -d;
		r[1][0] = d;
		*/
		s = r.transpose() * m;

		if (s != s)
		{
			std::cerr << "NAN in polar decom" << std::endl;
		}
	}

	inline void polarDecomposition3D(const Eigen::Matrix3f& m, Eigen::Matrix3f& s, Eigen::Matrix3f& r)
	{
		float x = m(1, 1) + m(0, 0); // trace
		float y = m(1, 0) - m(0, 1);
		float scale = 1.0f / std::sqrt(x * x + y * y);
		float c = x * scale;
		float d = y * scale;
		r << c, -d,
			d, c;

		s = r.transpose() * m;

		if (s != s)
		{
			std::cerr << "NAN in polar decom" << std::endl;
		}
	}

	
	// Helper class for functions
	class utilF
	{
	public:
		// Create a new directory using Boost. Ruturns false if unable to create
		bool static createDir(const std::string& dirName);
		// Write an image to the path specified
		void static writeImageToDisk(char const* filename, int id, int w, int h, int comp, const void* data);
		// Export Frames as gif
		void static writeFramesAsGif(const std::vector < FrameSBF<float>>& frames, SimVisualizer& viewer, int milisPerFrame, std::string fileName);
	};
}
#endif // UTILS_H