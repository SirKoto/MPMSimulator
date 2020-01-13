#pragma once
#ifndef _SIM3D_
#define _SIM3D_

#include <glm/glm.hpp>
#include <vector>
#include <Eigen/Dense> // include all core and algebra headers

class Simulator_3D
{
public:
	// Types of energy models for hyperplasticity
	enum class HYPERELASTICITY
	{
		COROTATED,
		NEOHOOKEAN,
		SAND,
		LINEAR
	};

	// Constructor
	Simulator_3D(HYPERELASTICITY mode = HYPERELASTICITY::COROTATED);

	// Destructor
	~Simulator_3D()
	{
		delete[] grid, physicsGrid;
	}
	// Returns the number of particles. Dumps the position of each particle [0,grid_size]
	unsigned int dumpPositions(float* positions) const;

	// Returns the number of particles. Dumps the position of each particle [0,1]
	unsigned int dumpPositionsNormalized(float* positions) const;

	// Main function. Make a time advance of dt
	void step(float dt);

	// Add new particle. The position can be from [0, grid_size]
	void addParticle(const glm::vec3& pos, const glm::vec3& v = glm::vec3(0), int material = 0);

	// Add new particle. The position can be from [0, 1]
	void addParticleNormalized(const glm::vec3& pos, const glm::vec3& v = glm::vec3(0), int material = 0);

	// Remove all data from the pysicsGrid
	void clearPhysics();

	// Create flat physics surface at height [0,1]
	void setPhysicsFlat(float height);
	
	// Create two symetric slopes in the pysics grid. Given a height, and initial height
	// And the width of the hole. Depth determines the number of layers in the grid filled
	void setPhysicSlopes(float height, float initialH, float holeWidth, int depth = 1);

	// Create vertical wall in the XY plane
	void setPhysicsZWall(float zmin, float zmax, int depth = 1);

	// Get young modulus from material in position pos
	float getYoung(int pos = 0) { return v_properties[pos].young; }

	float getNu(int pos = 0) { return v_properties[pos].nu; }

	float getHardening(int pos = 0) { return v_properties[pos].hardening; }

	float getVolume(int pos = 0) { return v_properties[pos].volume; }

	float getMass(int pos = 0) { return v_properties[pos].mass; }

	float getPlasticity(int pos = 0) { return static_cast<float>(v_properties[pos].plasticity); }

	float getT_C(int pos = 0) { return v_properties[pos].t_c; }

	float getT_S(int pos = 0) { return v_properties[pos].t_s; }

	HYPERELASTICITY getMode() { return mode; }
	// Get the number of materials in the system actually
	int getNumMaterials() { return static_cast<int>(v_properties.size()); }

	// add a new material
	int addNewMaterial(float young, float nu, float hardening, float volume, float mass, bool plasticity, float t_c, float t_s);

private:

	const HYPERELASTICITY mode;

	const float grid_size; // Size of the grid
	const float d_size;    // Inverse of the size

	// Gravity constant
	const Eigen::Array3f g = Eigen::Array3f(0.0f, -10.0f, 0.0f);

	// Struct defining the materials
	struct property
	{
		const float young, nu, mu, lambda, hardening, volume, mass, t_c, t_s, p_c, p_s;
		const bool plasticity;
		property(float young, float nu, float hardening, float volume, float mass, bool plasticity = true,
			float t_c = 2.5e-2f, float t_s = 7.5e-3f)
			: young(young), nu(nu), hardening(hardening), volume(volume), mass(mass),
				mu(young / (2 * (1 + nu))),
				lambda(young * nu / ((1 + nu) * (1 - 2 * nu))),
			t_c(t_c), t_s(t_s), p_c(0.6f), p_s(20.f),
			plasticity(plasticity)
		{}
	};

	// vector of materials
	std::vector<property> v_properties;

	// Structure defining each particle
	struct Particle
	{
		Eigen::Array3f pos, v; // position and velocity

		Eigen::Matrix3f F, C; // Deformation gradient and C from APIC

		float Jp; // Determinat of plastic F (Jacobian).

		const int prop_id; // material id

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

	// Using power of 2 grid sizes... faster indexing
	//#define getInd(x, y, z) (((((x) << 6) | (y)) << 6) | (z))
	#define getInd(x, y, z) (((((x) << 7) | (y)) << 7) | (z))

	std::vector<Particle> particles; // vector of particles
	Eigen::Array4f* grid; // Each element (v.x, v.y, v.z, mass)
	Eigen::Array3f* physicsGrid; // Grid with the normals of the pysics

	// Function to compute the outer product of two 3D arrays, and sum the resulting matrix to another
	inline static void SumOuterProduct(Eigen::Matrix3f& r, const Eigen::Array3f& a, const Eigen::Array3f& b);
};


#endif //_SIM3D_
