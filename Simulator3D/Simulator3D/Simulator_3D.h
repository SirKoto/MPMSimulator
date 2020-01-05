#pragma once
#ifndef _SIM3D_
#define _SIM3D_

#include <glm/glm.hpp>
#include <vector>
#include <Eigen/Dense> // include all core and algebra headers

class Simulator_3D
{
public:

	enum class HYPERELASTICITY
	{
		COROTATED,
		NEOHOOKEAN,
		SAND
	};


	Simulator_3D(HYPERELASTICITY mode = HYPERELASTICITY::COROTATED);

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

	void setPhysicsZWall(float zmin, float zmax, int depth = 1);

	float getYoung(int pos = 0) { return v_properties[pos].young; }

	float getNu(int pos = 0) { return v_properties[pos].nu; }

	float getHardening(int pos = 0) { return v_properties[pos].hardening; }

	float getVolume(int pos = 0) { return v_properties[pos].volume; }

	float getMass(int pos = 0) { return v_properties[pos].mass; }

	int getNumMaterials() { return static_cast<int>(v_properties.size()); }

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
		const float young, nu, mu, lambda, hardening, volume, mass, t_c, t_s, p_c, p_s;

		property(float young, float nu, float hardening, float volume, float mass)
			: young(young), nu(nu), hardening(hardening), volume(volume), mass(mass),
				mu(young / (2 * (1 + nu))),
				lambda(young * nu / ((1 + nu) * (1 - 2 * nu))),
			t_c(2.5e-2f), t_s(7.5e-3f), p_c(0.6f), p_s(20.f)
		{}
	};

	std::vector<property> v_properties;

	struct Particle
	{
		Eigen::Array3f pos, v; // posicio i velocitat de la particula

		Eigen::Matrix3f F, C; // Gradient de deformació i APIC

		float Jp; // Determinat de F (Jacobian) indica la deformacio del volum

		const int prop_id;

		Particle() : Jp(1.0f), prop_id(0)
		{
			pos = Eigen::Array3f::Zero();
			v = Eigen::Array3f::Zero();

			F = Eigen::Matrix3f::Identity();
			C = Eigen::Matrix3f::Zero();
		}

		Particle(const Eigen::Array3f& x, Eigen::Array3f& v, int prop_id = 0) :
			pos(x),
			v(v),
			Jp(1),
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

	inline static void SumOuterProduct(Eigen::Matrix3f& r, const Eigen::Array3f& a, const Eigen::Array3f& b);
};


#endif //_SIM3D_
