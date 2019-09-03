#include "Simulator_2D.h"

#include "Utils.h"

#include <algorithm>

#include <iostream>


Simulator_2D::Simulator_2D(float E, float nu) :
	mu_0(E / (2 * (1 + nu))),
	lambda_0(E * nu / ((1 + nu) * (1 - 2 * nu))),
	width(80),
	height(80)
{ 
	grid_size = glm::vec2(width, height);

	aspectR = static_cast<float>(width)/ height;

	d_size = 1.0f / grid_size;

	//particles = std::vector<Particle>(0);
	std::memset(grid, 0, sizeof(grid));
}

unsigned int Simulator_2D::dumpPositions(float* positions) const
{
	unsigned int i = 0;
	const unsigned int size = static_cast<unsigned int>(particles.size()) / 2;
	for (; i < size; ++i)
	{
		positions[2 * i] = particles[i].pos.x;// *grid_size.x;
		positions[2 * i + 1] = particles[i].pos.y;// *grid_size.y;
	}
	return 2 * size;
}

unsigned int Simulator_2D::dumpPositionsNormalized(float* positions) const
{
	unsigned int i = 0;
	const unsigned int size = static_cast<unsigned int>(particles.size()) >> 1; // div 2
	for (; i < size; ++i)
	{
		// TODO: posar que en un sol pas copii tot (ja que un vec2 en teoria son els dos floats seguits)
		positions[2 * i] = particles[i].pos.x * d_size.x;// *grid_size.x;
		positions[2 * i + 1] = particles[i].pos.y * d_size.y;// *grid_size.y;
	}
	return 2 * size;
}

void Simulator_2D::step(float dt)
{
	// all grid with 0's, velocity and mass
	std::memset(grid, 0, sizeof(grid));

	// Particle to grid
	for (auto& p : this->particles)
	{
		// compute the center 
		glm::vec2 cell_idx = glm::floor(p.pos /* * grid_size*/);
		glm::vec2 distFromCenter = (p.pos /* * grid_size*/ - cell_idx) - 0.5f; // center at point 0,0

		glm::vec2 weights[3] = {
		  0.5f * glm::pow(0.5f - distFromCenter, glm::vec2(2.0f)),
		  0.75f - glm::pow(distFromCenter, glm::vec2(2.0f)),
		  0.5f * glm::pow(distFromCenter + 0.5f, glm::vec2(2.0f))
		};

		// Lamé parameters
		float e = std::exp(hardening * (1.0f - p.J));
		float mu = mu_0 * e;
		float lambda = lambda_0 * e;

		float J = glm::determinant(p.F);
		// Euler explicit time integration
		// Looking for stress


		glm::mat2 s, r;
		utils::polarDecomposition2D(p.F, s, r); // Decompose the deformation gradient in a rotation and an scale


		//Corotated constitucional model     // [http://mpm.graphics Eqn. 52]
		glm::mat2 PF = 2.0f * mu * (p.F - r) * glm::transpose(p.F) + lambda * (J - 1.0f) * J;

		//const glm::vec2 Dinv = 4.0f * grid_size * grid_size;
		// Identity sclae by inverse derivate
		//const glm::mat2 DinvM = glm::mat2(Dinv.x, 0.0f, 0.0f, Dinv.y);
		//EQn. 173
		glm::mat2 stress = - (dt * volume) * (4.0f * PF); // eq_16_term_0

		glm::mat2 affine = stress + mass * p.C;

		//P2G
		for (int i = -1; i < 2; ++i)
		{
			for (int j = -1; j < 2; ++j)
			{
				// cell_x is the idx of the selected cell to update
				glm::vec2 cell_x = cell_idx + glm::vec2(i, j);

				// No hauria de fer falta la seguent linea
				// if (cell_x.x < 0 || cell_x.y < 0 || cell_x.x > height || cell_x.y > width) continue;


				// cell_distance to the particle in the corresponding updating cell
				glm::vec2 cell_dist = (cell_x - p.pos ) /* * grid_size*/ + 0.5f;
				// cell_dist *= d_size; // to [0,1]
				const float w = weights[i + 1].x * weights[j + 1].y;

				//TODO: optimitzar multiplicant w nomes per mass
				const glm::vec3 moment_mass(p.v * mass, mass); // moment and particle mass

				grid[static_cast<int>(cell_x.x)][static_cast<int>(cell_x.y)] +=
					w * (moment_mass + glm::vec3(affine * cell_dist, 0));



				if (grid[static_cast<int>(cell_x.x)][static_cast<int>(cell_x.y)] != grid[static_cast<int>(cell_x.x)][static_cast<int>(cell_x.y)]) {
					std::cerr << "veri bigu problem" << std::endl;
				}
			}
		}
	}

	// Process grid
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			glm::vec3& cell = grid[i][j];
			// cell is (v.x, v.y, mass)
			// only if there is mass
			if (cell.z > 0)  // TODO: posar != per a eficiencia
			{
				// normalize by mass
				// momentum to velocity
				cell /= cell.z;
				cell += glm::vec3(dt * g, 0);

				//float x = static_cast<float>(i) / height;
				//float y = static_cast<float>(j) / width;
				// resolution and borders
				//if (x < boundary || x > 1 - boundary || y > 1 - boundary)
				if(i < 2 || i > height - 3)
				{
					cell.x = 0.0f;
				}
				if (j < 2 || j > width - 3)
				{
					cell.y = 0.0f;
				}
			}
		}
	}

	// Grid to particle
	for (auto& p : particles)
	{
		// compute the center 
		glm::vec2 cell_idx = glm::floor(p.pos /* * grid_size*/);
		glm::vec2 distFromCenter = (p.pos /* * grid_size*/ - cell_idx) - 0.5f; // center at point 0,0

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

				glm::vec2 cell_x = cell_idx + glm::vec2(i, j);
				glm::vec2 cell_dist = (cell_x - p.pos)/* * grid_size */+ 0.5f;
				//if (cell_x.x < 0 || cell_x.y < 0 || cell_x.x > height || cell_x.y > width) continue;

				const float w = weights[i + 1].x * weights[j + 1].y;
				const glm::vec2 cell_v = grid[static_cast<int>(cell_x.x)][static_cast<int>(cell_x.y)];

				if (cell_v.x != cell_v.x || cell_v.y != cell_v.y)
				{
					std::cerr << "preb v nan" << std::endl;
				}

				p.v += w * cell_v;

				// apic, eq 10
				p.C += utils::outerProduct(w * cell_v, cell_dist);
			}
		}

		p.C *= 4.0f;

		// advect particles
		if (p.v.x != p.v.x || p.v.y != p.v.y)
		{
			std::cerr << "We have a NAN" << std::endl;
		}

		p.pos += dt * p.v;


		// safety clamp!!

		p.pos = glm::clamp(p.pos, glm::vec2(1, 1), grid_size - 2.0f);

		// update F gradient
		glm::mat2 F = (glm::mat2(1.0f) + dt * p.C) * p.F;

		glm::mat2 svd_u, svd_e, svd_v;
		utils::singularValueDecomposition(F, svd_u, svd_e, svd_v);

		glm::mat2 test = svd_u* svd_e* svd_v;
		// Snow paper elasticiy constrains
		for (int i = 0; i < 2; ++i) {
			svd_e[i][i] = glm::clamp(svd_e[i][i], 1.0f - 2.5e-2f, 1.0f + 7.5e-3f);

		}

		/*if (!_finite(F[0][0]) || !_finite(F[0][1]) || !_finite(F[1][0]) || !_finite(F[1][1]))
		{
			std::cerr << "another NAN" << std::endl;
		}*/

		float oldJ = glm::determinant(F);
		F = svd_u * svd_e * glm::transpose(svd_v);

		float newJ = glm::clamp(p.J * oldJ / glm::determinant(F), 0.6f, 20.0f);

		p.F = F;
		p.J = newJ;
	}
}

void Simulator_2D::addParticle(const glm::vec2& pos, const glm::vec2& v)
{
	Particle p(pos /* * d_size*/, v);
	particles.push_back(p);
}

