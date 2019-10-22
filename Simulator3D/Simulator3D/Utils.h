#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Eigen/Dense>

#include <iostream>
#include <string>


#define MSG(m) std::cout << m << std::endl 
#define TMSG(m) std::cout << "\t" << m << std::endl
namespace utils
{
	static unsigned int SCR_WIDTH = 800;
	static unsigned int SCR_HEIGHT = 600;

	static float DeltaTime = 0.0f;
	static float LastFrame = 0.0f;
	constexpr unsigned int maxParticles = 300;

	constexpr unsigned int sizG = 1;

	inline float updateTime()
	{
		float currentFrame = (float)glfwGetTime();
		utils::DeltaTime = currentFrame - utils::LastFrame;
		utils::LastFrame = currentFrame;
		return currentFrame;
	}

	inline void SumOuterProduct(Eigen::Matrix2f& r, const Eigen::Array2f& a, const Eigen::Array2f& b)
	{

		r(0,0) += a[0] * b[0];
		r(0,1) += a[0] * b[1];
		r(1,0) += a[1] * b[0];
		r(1,1) += a[1] * b[1];
	}

	inline void SumOuterProduct(Eigen::Matrix3f& r, const Eigen::Array3f& a, const Eigen::Array3f& b)
	{

		r(0, 0) += a[0] * b[0];
		r(0, 1) += a[0] * b[1];
		r(0, 2) += a[0] * b[2];

		r(1, 0) += a[1] * b[0];
		r(1, 1) += a[1] * b[1];
		r(1, 2) += a[1] * b[2];

		r(2, 0) += a[2] * b[0];
		r(2, 1) += a[2] * b[1];
		r(2, 2) += a[2] * b[2];
	}

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

	constexpr GLubyte CubeIndices[] =	   
							   { 0,1,2, 2,3,0,   // 36 of indices
								 0,3,4, 4,5,0,
								 0,5,6, 6,1,0,
								 1,6,7, 7,2,1,
								 7,4,3, 3,2,7,
								 4,7,6, 6,5,4 };

	constexpr GLfloat CubeVertices[] = 
							 {  sizG, sizG, 0.0f,
								0.0f, sizG, 0.0f,
								0.0f, 0.0f, 0.0f, 
								sizG, 0.0f, 0.0f,
								sizG, 0.0f, sizG,
								sizG, sizG, sizG,
								0.0f, sizG, sizG,
								0.0f, 0.0f, sizG
								};

	class utilF
	{
	public:

		void static writeImageToDisk(char const* filename, int id, int w, int h, int comp, const void* data);
	};
}
#endif // UTILS_H