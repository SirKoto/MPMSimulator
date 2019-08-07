#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class Simulator_2D
{
public:
	Simulator_2D(float E = 1e4f, float nu = 0.2f);

	// Returns the number of particles dumped in positions
	unsigned int dumpPositions(float* positions) const;

	void step(float dt);

	void addParticle(const glm::vec2& pos, const glm::vec2& v = glm::vec2(0));

private:
	const float mu_0, lambda_0;
	static const unsigned int width = 80, height = 80;
	const float hardening = 10.0f;
	const float volume = 1.0f;
	const float m = 1.0f; // massa
	const glm::vec2 g = glm::vec2(0, -10.0f);
	struct Particle
	{
		glm::vec2 pos, v; // posicio i velocitat de la particula

		glm::mat2 F, C; // Gradient de deformació i APIC

		float J; // Determinat de F (Jacobian)

		Particle(const glm::vec2& x, const glm::vec2& v = glm::vec2(0)) :
			pos(x),
			v(v),
			F(1),
			C(0),
			J(1){}
	};

	std::vector<Particle> particles;
	glm::vec3 grid[80 + 1][80 + 1]; // v.x, v.y, mass

};

