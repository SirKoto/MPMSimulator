#pragma once
#ifndef _SIM3D_
#define _SIM3D_

#include <glm/glm.hpp>
#include <vector>
#include <Eigen/Dense> // include all core and algebra headers

#include "Utils.h"

class Simulator_3D
{
public:
	Simulator_3D(float E = 3.5e4f, float nu = 0.42f);

	~Simulator_3D()
	{
		delete[] grid;
	}
	// Returns the number of particles dumped in positions
	unsigned int dumpPositions(float* positions) const;

	unsigned int dumpPositionsNormalized(float* positions) const;

	void step(float dt);

	void addParticle(const glm::vec3& pos, const glm::vec3& v = glm::vec3(0));

	void addParticleNormalized(const glm::vec3& pos, const glm::vec3& v = glm::vec3(0));

private:

	const float mu_0, lambda_0;

	const float boundary = 0.05f;


	const float grid_size;
	const float d_size; // derivate of the size

	Eigen::Array3f minBorder, maxBorder;

	const float hardening = 0.3f;
	const float volume = 0.3f;
	const float mass = 1.0f; // massa
	const Eigen::Array3f g = Eigen::Array3f(0.0f, -10.0f, 0.0f);
	
	//Eigen::JacobiSVD<Eigen::Matrix3f, Eigen::NoQRPreconditioner> svd;

	struct Particle
	{
		Eigen::Array3f pos, v; // posicio i velocitat de la particula

		Eigen::Matrix3f F, C; // Gradient de deformaci� i APIC

		float J; // Determinat de F (Jacobian) indica la deformacio del volum

		Particle() : J(1.0f)
		{
			pos = Eigen::Array3f::Zero();
			v = Eigen::Array3f::Zero();

			F = Eigen::Matrix3f::Identity();
			C = Eigen::Matrix3f::Zero();
		}

		Particle(const Eigen::Array3f& x, Eigen::Array3f& v) :
			pos(x),
			v(v),
			J(1)
		{
			F = Eigen::Matrix3f::Identity();
			C = Eigen::Matrix3f::Zero();
		}
	};

	// Get index = 128 * x + y
	#define getInd(x, y, z) (((((x) << 7) | (y)) << 7) | (z))

	std::vector<Particle> particles;
	Eigen::Array4f* grid;// [128 * 128 * 128] ; // v.x, v.y, v.z, mass

};


#endif //_SIM3D_
