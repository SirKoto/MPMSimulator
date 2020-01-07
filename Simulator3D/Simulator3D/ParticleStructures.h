#pragma once

#ifndef _PARTICLESTRUCTURES_
#define _PARTICLESTRUCTURES_

#include "Simulator_3D.h"
#include <random>
#include <cmath>

// Header with model creations to simulate
namespace ps
{
	// Create 3 boxes at the same height
	int create3BoxesFilledHomo(Simulator_3D& sim, glm::vec3*& p_col, int _n_particles = utils::maxParticles, const glm::vec3& velocity = glm::vec3(0.0f))
	{
		int p_perBox = _n_particles / 3;
		int p_perDimension = static_cast<int>(std::floor(std::cbrt(p_perBox)));
		p_perBox = static_cast<int>(std::pow(p_perDimension, 3));
		int n_particles = p_perBox * 3;

		float dx = 0.2f / static_cast<float>(p_perDimension);

		p_col = new glm::vec3[n_particles];

		glm::vec3 inPos[] = { glm::vec3(0.4f), glm::vec3(0.6f), glm::vec3(0.2f) };
		glm::vec3 color[] = { glm::vec3(1,1,0), glm::vec3(1,0,1), glm::vec3(0,1,1) };
		for (int p = 0; p < 3; p++)
		{
			float x = inPos[p].x;

			for (int i = 0; i < p_perDimension; ++i, x += dx)
			{
				float y = inPos[p].y;

				for (int j = 0; j < p_perDimension; ++j, y += dx)
				{
					float z = inPos[p].z;

					for (int k = 0; k < p_perDimension; ++k, z += dx)
					{
						sim.addParticleNormalized(glm::vec3(x, y, z), velocity);
					}
				}
			}

			// fill color
			for (int i = p * p_perBox; i < (p + 1) * p_perBox; ++i)
			{
				p_col[i] = color[p];
			}
		}

		return n_particles;
	}

	// Create 3 boxes at diferent positions
	int create3BoxesSeparatedFilledHomo(Simulator_3D& sim, glm::vec3*& p_col, int _n_particles, const glm::vec3& velocity,
		const int materials[3])
	{
		int p_perBox = _n_particles / 3;
		int p_perDimension = static_cast<int>(std::floor(std::cbrt(p_perBox)));
		p_perBox = static_cast<int>(std::pow(p_perDimension, 3));
		int n_particles = p_perBox * 3;

		constexpr float width = .15f;
		constexpr float dw = 1.0f / 4.0f;


		float dx = width / static_cast<float>(p_perDimension);

		constexpr glm::vec3 inPos[] = { glm::vec3(dw, 0.5f, 0.5f) - width * 0.5f,
			glm::vec3(2 * dw, 0.5f, 0.5f) - width * 0.5f,
			glm::vec3(3 * dw, 0.5f, 0.5f) - width * 0.5f };
		constexpr glm::vec3 color[] = { glm::vec3(1,1,0), glm::vec3(1,0,1), glm::vec3(0,1,1) };

		p_col = new glm::vec3[n_particles];

		for (int p = 0; p < 3; p++)
		{
			float x = inPos[p].x;

			for (int i = 0; i < p_perDimension; ++i, x += dx)
			{
				float y = inPos[p].y;

				for (int j = 0; j < p_perDimension; ++j, y += dx)
				{
					float z = inPos[p].z;

					for (int k = 0; k < p_perDimension; ++k, z += dx)
					{
						sim.addParticleNormalized(glm::vec3(x, y, z), velocity, materials[p]);
					}
				}
			}

			// fill color
			for (int i = p * p_perBox; i < (p + 1) * p_perBox; ++i)
			{
				p_col[i] = color[p];
			}
		}

		return n_particles;
	}

	// Unique big BB filled with all particles at the same distances
	int createBoxFilledHomo(Simulator_3D& sim, glm::vec3*& p_col, int _n_particles = utils::maxParticles,
		float x0 = 0.1f, float x1 = 0.9f, float y0 = 0.65f, float y1 = 0.98f, float z0 = 0.3f, float z1 = 0.7f)
	{
		const float vol = (x1 - x0) * (y1 - y0) * (z1 - z0);
		const float p = _n_particles * (1 / vol);
		const float pDim = std::cbrtf(p);
		const float dx = 1 / pDim;
		int n_particles = 0;
		for (float x = x0; x < x1; x += dx)
		{
			for (float y = y0; y < y1; y += dx)
			{
				for (float z = z0; z < z1; z += dx)
				{
					++n_particles;
				}
			}
		}
		p_col = new glm::vec3[n_particles];

		glm::vec3 color[] = { glm::vec3(1,1,0), glm::vec3(1,0,1), glm::vec3(0,1,1) };

		const float dy = (y1 - y0) / 3;
		int i = 0;
		for (float x = x0; x < x1; x += dx)
		{
			for (float y = y0; y < y1; y += dx)
			{
				for (float z = z0; z < z1; z += dx, i++)
				{
					sim.addParticleNormalized(glm::vec3(x, y, z));
					p_col[i] = color[y > y0 + 2.f * dy ? 0 : y > y0 + dy ? 1 : 2];
				}
			}
		}

		return n_particles;
	}

	// Unique big BB filled randomly
	int createBoxFilled(Simulator_3D& sim, glm::vec3*& p_col, int n_particles = utils::maxParticles)
	{
		p_col = new glm::vec3[n_particles];

		// add random particles
		std::mt19937 mt_rng(42 + 1);
		std::uniform_real_distribution<float> disX(0.1f, 0.9f);
		std::uniform_real_distribution<float> disZ(0.3f, 0.7f);
		std::uniform_real_distribution<float> disY(0.4f, 0.8f);

		const float dy = (0.8f - 0.4f) / 3;
		for (int i = 0; i < n_particles; ++i)
		{
			float x = disX(mt_rng);
			float y = disY(mt_rng);
			float z = disZ(mt_rng);
			sim.addParticleNormalized(glm::vec3(x, y, z), glm::vec3(0.0f, 1.0f, -2.0f));

			p_col[i] = y > 0.4f + 2.f * dy ? glm::vec3(0.0f, 1.0f, 0.0f) : y < 0.4f + dy ? glm::vec3(0.0f, 1.0f, 1.0f) : glm::vec3(1.0f, 0.0f, 1.0f); // color according to height
		}

		return n_particles;
	}

	// Two colliding spheres generated such that collide with diferent heights
	int create2CollidingSpheres(Simulator_3D& sim, glm::vec3*& p_col, int _n_particles, float radius, float velocity, const int materials[2])
	{
		int p_perSphere = _n_particles / 2;
		int height = 2 * static_cast<int>(std::floor(std::cbrt(p_perSphere * 3.0 / (4.0 * glm::pi<double>()))));
		float interRadius = radius / height;

		// lambda function to create a sphere
		auto fill = [&height, &interRadius, &sim](const glm::vec3 &center, const glm::vec3 &v, int mat) {
			int count = 0;
			for (int i = 0; i < height; ++i)
			{
				int h = i - height / 2;
				int cw = static_cast<int>(std::sqrt(static_cast<float>((height / 2) * (height / 2) - h * h)));
				int w = 2 * cw;
				for (int j = 0; j < w; ++j)
				{
					int d = j - cw;
					int cz = static_cast<int>(std::sqrt(static_cast<float>(cw * cw - d * d)));
					int z = 2 * cz;

					for (int k = 0; k < z; ++k)
					{
						int t = k - cz;
						glm::vec3 pos(d, h, t);
						sim.addParticleNormalized(center + pos * interRadius, v, mat);

						++count;
					}
				}
			}
			return count;
		};
		const glm::vec3 center[2] = { glm::vec3(0.2, 0.53, 0.48), glm::vec3(0.8, 0.5, 0.5) };

		int count = fill(center[0],  glm::vec3(velocity, 0.0f,0.0f), materials[0]);
		count += fill(center[1], glm::vec3(-velocity, 0.0f, 0.0f), materials[1]);

		p_col = new glm::vec3[count];

		for (int i = 0; i < count/2; ++i)
		{
			p_col[i] = glm::vec3(0.f, 0.1f, 0.9f);
		}
		for (int i = count / 2; i < count; ++i)
		{
			p_col[i] = glm::vec3(0.1f, 0.3f, 0.9f);
		}
		return count;

	}
}

#endif // !_PARTICLESTRUCTURES_