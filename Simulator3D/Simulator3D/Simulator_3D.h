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

	void addParticle(const glm::vec3& pos, const glm::vec3& v = glm::vec3(0), int material = 0);

	void addParticleNormalized(const glm::vec3& pos, const glm::vec3& v = glm::vec3(0), int material = 0);

	void clearPhysics();

	void setPhysicsFlat(float height);

	void setPhysicsHourglass(float extRadius, float intRadius, float separation);
	
	void setPhysicSlopes(float height, float initialH, float holeWidth, int depth = 1);

	float getYoung(int pos = 0) { return v_properties[pos].young; }

	float getNu(int pos = 0) { return v_properties[pos].nu; }

	float getHardening(int pos = 0) { return v_properties[pos].hardening; }

	float getVolume(int pos = 0) { return v_properties[pos].volume; }

	float getMass(int pos = 0) { return v_properties[pos].mass; }

	int addNewMaterial(float young, float nu, float hardening, float volume = 1.0f, float mass = 1.0f);

private:

	const HYPERELASTICITY mode;

	const float boundary = 0.05f;


	const float grid_size;
	const float d_size; // derivate of the size

	Eigen::Array3f minBorder, maxBorder;

	const Eigen::Array3f g = Eigen::Array3f(0.0f, -10.0f, 0.0f);

	struct property
	{
		const float young, nu, mu, lambda, hardening, volume, mass;

		property(float young, float nu, float hardening, float volume, float mass)
			: young(young), nu(nu), hardening(hardening), volume(volume), mass(mass),
				mu(young / (2 * (1 + nu))),
				lambda(young * nu / ((1 + nu) * (1 - 2 * nu)))
		{}
	};

	std::vector<property> v_properties;

	struct Particle
	{
		Eigen::Array3f pos, v; // posicio i velocitat de la particula

		Eigen::Matrix3f F, C; // Gradient de deformaci� i APIC

		float J; // Determinat de F (Jacobian) indica la deformacio del volum

		const int prop_id;

		Particle() : J(1.0f), prop_id(0)
		{
			pos = Eigen::Array3f::Zero();
			v = Eigen::Array3f::Zero();

			F = Eigen::Matrix3f::Identity();
			C = Eigen::Matrix3f::Zero();
		}

		Particle(const Eigen::Array3f& x, Eigen::Array3f& v, int prop_id = 0) :
			pos(x),
			v(v),
			J(1),
			prop_id(prop_id)
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
