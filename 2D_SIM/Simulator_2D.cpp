#include "Simulator_2D.h"
#include "Utils.h"
#include <algorithm>

Simulator_2D::Simulator_2D(float E, float nu) :
	mu_0(E / (2 * (1 + nu))),
	lambda_0(E* nu / ((1 + nu) * (1 - 2 * nu)))
{ }

unsigned int Simulator_2D::dumpPositions(float* positions) const
{
	unsigned int i = 0;
	unsigned int size = static_cast<unsigned int>(particles.size()) / 2;
	for (; i < size; ++i)
	{
		positions[2 * i] = particles[i].pos.x;
		positions[2 * i + 1] = particles[i].pos.y;
	}
	return i;
}

void Simulator_2D::step(float dt)
{
	// all grid with 0's
	std::memset(grid, 0, sizeof(grid));

	// Particle to grid
	for (auto& p : particles)
	{
		// compute the center 
		glm::vec2 cell_idx = glm::floor(p.pos);
		glm::vec2 distFromCenter = p.pos - cell_idx - 0.5f; // center at point 0,0

		glm::vec2 weights[3] = {
		  0.5f * glm::pow(0.5f - distFromCenter,glm::vec2(2.0f)),
		  0.75f - glm::pow(distFromCenter, glm::vec2(2.0f)),
		  0.5f * glm::pow(distFromCenter + 0.5f, glm::vec2(2.0f))
		};

		// Lamé parameters
		float e = std::exp(hardening * (1.0f - p.J));
		float mu = mu_0 * e;
		float lambda = lambda_0 * e;

		glm::mat2 s, r;
		utils::polarDecomposition2D(p.F, s, r); // Decompose the deformation gradient in a rotation and an scale

		float J = glm::determinant(p.F);
		// Euler explicit time integration
		// Looking for stress
		//Corotated constitucional model     // [http://mpm.graphics Eqn. 52]
		glm::mat2 PF = (2 * mu * (p.F - r) * glm::transpose(p.F) + lambda * (J - 1) * J);
		//EQn. 173
		glm::mat2 stress = - dt * volume * 4.0f * PF; // eq_16_term_0

		glm::mat2 affine = stress + m * p.C;

		//P2G
		for (int i = -1; i < 2; ++i)
		{
			for (int j = -1; j < 2; ++j)
			{
				glm::vec2 dpos = (glm::vec2(i, j) - distFromCenter);
				const float w = weights[i + 1].x * weights[j + 1].y;
				const glm::vec3 data(p.v * m, m); // moment and particle mass
				grid[static_cast<int>(cell_idx.x) + i][static_cast<int>(cell_idx.y) + j] += w * (data + glm::vec3(affine * dpos, 0));
			}
		}
	}

	// Process grid
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			glm::vec3& cell = grid[i][j];

			// only if there is mass
			if (cell.z != 0)
			{
				// normalize by mass
				// momentum to velocity
				cell /= cell.z;
				cell += dt * glm::vec3(g, 0);

				// resolution and borders
				if (i < 2 || i > width - 2)
					cell.x = 0;
				if(j < 2)
					cell.y = std::max(0.0f, cell.y);
			}
		}

		// Grid to particle
		for (Particle& p : particles)
		{
			// compute the center 
			glm::vec2 cell_idx = glm::floor(p.pos);
			glm::vec2 distFromCenter = p.pos - cell_idx - 0.5f; // center at point 0,0

			glm::vec2 weights[3] = {
			  0.5f * glm::pow(0.5f - distFromCenter,glm::vec2(2.0f)),
			  0.75f - glm::pow(distFromCenter, glm::vec2(2.0f)),
			  0.5f * glm::pow(distFromCenter + 0.5f, glm::vec2(2.0f))
			};

			p.C = glm::mat2(0.0f);
			p.v = glm::vec2(0.0f);

			for (int i = -1; i < 2; ++i)
			{
				for (int j = -1; j < 2; ++j)
				{
					glm::vec2 dpos = (glm::vec2(i, j) - distFromCenter);
					const float w = weights[i + 1].x * weights[j + 1].y;
					glm::vec2 cell = grid[static_cast<int>(cell_idx.x) + i][static_cast<int>(cell_idx.y) + j];
					p.v += w * cell;

					// apic
					p.C += 4.0f * utils::outerProduct(w * cell, dpos);
				}
			}

			// advect particles
			p.pos += dt * p.v;

			glm::mat2 F = (glm::mat2(1.0f) + dt * p.C) * p.F;



			p.F = F;
			p.J = glm::determinant(F);

		}
	}

}

void Simulator_2D::addParticle(const glm::vec2& pos, const glm::vec2& v)
{
	Particle p(pos, v);
	particles.push_back(p);
}
