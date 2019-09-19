#pragma once
#ifndef _SIM2D_
#define _SIM2D_

#include <glm/glm.hpp>
#include <vector>
#include <Eigen/Dense> // include all core and algebra headers

class Simulator_2D
{
public:
	Simulator_2D(float E = 3.5e4f, float nu = 0.42f);

	// Returns the number of particles dumped in positions
	unsigned int dumpPositions(float* positions) const;

	unsigned int dumpPositionsNormalized(float* positions) const;

	void step(float dt);

	void addParticle(const glm::vec2& pos, const glm::vec2& v = glm::vec2(0));

	inline const float getAspectRatio() const { return this->aspectR; };


private:

	const float mu_0, lambda_0;

	const float boundary = 0.05f;
	const unsigned int width, height; 

	float aspectR;

	Eigen::Array2f grid_size;
	Eigen::Array2f d_size; // derivate of the size

	Eigen::Array2f minBorder, maxBorder;

	const float hardening = 0.3f;
	const float volume = 0.3f;
	const float mass = 1.0f; // massa
	const Eigen::Array2f g = Eigen::Array2f(0.0f, -10.0f);
	
	struct Particle
	{
		Eigen::Array2f pos, v; // posicio i velocitat de la particula

		Eigen::Matrix2f F, C; // Gradient de deformaci� i APIC

		float J; // Determinat de F (Jacobian) indica la deformacio del volum

		Particle() : J(1.0f)
		{
			pos = Eigen::Array2f::Zero();
			v = Eigen::Array2f::Zero();

			F = Eigen::Matrix2f::Identity();
			C = Eigen::Matrix2f::Zero();
		}

		Particle(const Eigen::Array2f& x, Eigen::Array2f& v) :
			pos(x),
			v(v),
			J(1)
		{
			F = Eigen::Matrix2f::Identity();
			C = Eigen::Matrix2f::Zero();
		}
	};

	// Get index = 128 * x + y
	#define getInd(x, y) ((x << 7) | y)

	std::vector<Particle> particles;
	Eigen::Array3f grid[128 * 128]; // v.x, v.y, mass

};


#endif //_SIM2D_
