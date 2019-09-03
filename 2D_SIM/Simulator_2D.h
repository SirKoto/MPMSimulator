#pragma once
#ifndef _SIM2D_
#define _SIM2D_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class Simulator_2D
{
public:
	Simulator_2D(float E = 3.5e4f, float nu = 0.4f);

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

	glm::vec2 grid_size;
	glm::vec2 d_size; // derivate of the size

	const float hardening = 0.3f;
	const float volume = 0.3f;
	const float mass = 1.0f; // massa
	const glm::vec2 g = glm::vec2(0, -10.0f);
	
	struct Particle
	{
		glm::vec2 pos, v; // posicio i velocitat de la particula

		glm::mat2 F, C; // Gradient de deformació i APIC

		float J; // Determinat de F (Jacobian) indica la deformacio del volum

		Particle() : pos(0), v(0), F(1), C(0), J(1)
		{}

		Particle(const glm::vec2& x, const glm::vec2& v = glm::vec2(0)) :
			pos(x),
			v(v),
			F(1),
			C(0),
			J(1)
		{}
	};

	std::vector<Particle> particles;
	glm::vec3 grid[80 + 1][80 + 1]; // v.x, v.y, mass

};


#endif //_SIM2D_
