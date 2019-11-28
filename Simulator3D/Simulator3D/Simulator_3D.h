#pragma once
#ifndef _SIM3D_
#define _SIM3D_

#include <glm/glm.hpp>
#include <vector>
#include <Eigen/Dense> // include all core and algebra headers

#include "Utils.h"

//#define COROTATED // if defined use corotated, else using Neo-hookean

class Simulator_3D
{
public:

	enum class HYPERELASTICITY
	{
		COROTATED,
		NEOHOOKEAN,
		SAND
	};


	Simulator_3D(float E = 3.5e4f, float nu = 0.42f, HYPERELASTICITY mode = HYPERELASTICITY::COROTATED);

	~Simulator_3D()
	{
		delete[] grid, physicsGrid;
	}
	// Returns the number of particles dumped in positions
	unsigned int dumpPositions(float* positions) const;

	unsigned int dumpPositionsNormalized(float* positions) const;

	void step(float dt);

	void addParticle(const glm::vec3& pos, const glm::vec3& v = glm::vec3(0));

	void addParticleNormalized(const glm::vec3& pos, const glm::vec3& v = glm::vec3(0));

	void clearPhysics();

	void setPhysicsFlat(float height);

	void setPhysicsHourglass(float extRadius, float intRadius, float separation);
	
	void setPhisicSlopes(float height, float initialH, float holeWidth, int depth = 1);

	float getYoung() { return young; }

	float getNu() { return nu; }

private:

	const HYPERELASTICITY mode;

	const float young, nu;

	const float mu_0, lambda_0;

	const float boundary = 0.05f;


	const float grid_size;
	const float d_size; // derivate of the size

	Eigen::Array3f minBorder, maxBorder;

	const float hardening = 10.0f;
	const float volume = 1.0f;
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

	//#define getInd(x, y, z) (((((x) << 6) | (y)) << 6) | (z))
	#define getInd(x, y, z) (((((x) << 7) | (y)) << 7) | (z))

	std::vector<Particle> particles;
	Eigen::Array4f* grid;// v.x, v.y, v.z, mass
	Eigen::Array3f* physicsGrid;
};


#endif //_SIM3D_
