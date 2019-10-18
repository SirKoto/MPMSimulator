#include "Simulator_3D.h"

#include <algorithm>
#include <iostream>

//#define TIME_COUNT_FLAG
//#define G2P_FLAG


#ifdef TIME_COUNT_FLAG
#include <chrono>
#endif

Simulator_3D::Simulator_3D(float E, float nu) :
	mu_0(E / (2 * (1 + nu))),
	lambda_0(E* nu / ((1 + nu) * (1 - 2 * nu)))
{
	grid_size = 128;

	minBorder = Eigen::Array3f::Constant(0.0f + 1.0e-3f);
	maxBorder = Eigen::Array3f::Constant(1.0f - 1.0e-3f);

	d_size = 1.0f / grid_size;

	//particles = std::vector<Particle>(0);
	grid = new Eigen::Array4f[(128 * 128 * 128)];
	std::memset(grid, 0, (128 * 128 * 128 * sizeof(Eigen::Array4f)));

	svd = Eigen::JacobiSVD<Eigen::Matrix3f, Eigen::NoQRPreconditioner>(3, 3, Eigen::ComputeFullU | Eigen::ComputeFullV);
}

unsigned int Simulator_3D::dumpPositions(float* positions) const
{
	unsigned int j = 0;
	const unsigned int size = static_cast<unsigned int>(particles.size());
	for (; j < size; j++)
	{
		const int i = 3 * j;

		positions[i + 0] = particles[j].pos.x() * grid_size;
		positions[i + 1] = particles[j].pos.y() * grid_size;
		positions[i + 2] = particles[j].pos.z() * grid_size;

	}
	return size;
}

unsigned int Simulator_3D::dumpPositionsNormalized(float* positions) const
{
	unsigned int j = 0;
	const unsigned int size = static_cast<unsigned int>(particles.size());
	for (; j < size; j++)
	{
		const int i = 3 * j;

		positions[i + 0] = particles[j].pos.x();
		positions[i + 1] = particles[j].pos.y();
		positions[i + 2] = particles[j].pos.z();

	}
	return size;
}

void Simulator_3D::step(float dt)
{
	// all grid with 0's, velocity and mass
	std::memset(grid, 0, (128 * 128 * 128 * sizeof(Eigen::Array4f)));

#ifdef TIME_COUNT_FLAG
	auto start = std::chrono::steady_clock::now();
#endif
	// Particle to grid
	for (auto& p : this->particles)
	{
		assert(	p.pos.x() > 0.0f && p.pos.x() < 1.0f && 
				p.pos.y() > 0.0f && p.pos.y() < 1.0f &&
				p.pos.z() > 0.0f && p.pos.z() < 1.0f);
		// compute the center 
		const Eigen::Array3f& cell_idxf = (p.pos * grid_size);
		const Eigen::Array3i cell_idx = cell_idxf.cast<int>(); // floor
		const Eigen::Array3f distFromCenter = ((p.pos * grid_size) - cell_idx.cast<float>()) - 0.5f; // center at point 0,0,0

		const Eigen::Array3f weights[3] = {
		  0.5f * (0.5f - distFromCenter).square(),
		  0.75f - (distFromCenter).square(),
		  0.5f * (distFromCenter + 0.5f).square()
		};

		// Lame parameters
		const float e = std::exp(hardening * (1.0f - p.J));
		const float mu = mu_0 * e;
		const float lambda = lambda_0 * e;

		const float J = (p.F).determinant();
		// Euler explicit time integration
		// Looking for stress


		svd.compute(p.F);

		const Eigen::Matrix3f r = svd.matrixU() * svd.matrixV().transpose();


		//Corotated constitucional model     // [http://mpm.graphics Eqn. 52]
		const Eigen::Matrix3f PF = 2.0f * mu * (p.F - r) * (p.F).transpose() + Eigen::Matrix3f::Constant(lambda * (J - 1.0f) * J);


		//EQn. 173
		const Eigen::Matrix3f stress = -(dt * volume) * (4.0f * grid_size * grid_size * PF); // eq_16_term_0

		const Eigen::Matrix3f affine = stress + mass * p.C;

		//P2G
		const Eigen::Array3i cell_x0 = cell_idx + Eigen::Array3i(-1, -1, -1);
		const Eigen::Vector3f cell_dist0 = ((cell_x0.cast<float>() - (p.pos * grid_size)) + 0.5f);

		{
			Eigen::Array4f moment_mass0 = (Eigen::Array4f() << p.v * mass, mass).finished(); // moment and particle mass
			moment_mass0.head<3>() += d_size * (affine * cell_dist0).array();

			const Eigen::Array3f kstep = affine.col(2) * d_size;
			const Eigen::Array3f jstep = (affine.col(1) * d_size).array() - (3.0f * kstep);
			const Eigen::Array3f istep = (affine.col(0) * d_size).array() - (3.0f * jstep);

			for (int i = -1; i < 2; ++i)
			{
				for (int j = -1; j < 2; ++j)
				{
					for (int k = -1; k < 2; ++k)
					{

						const float w = weights[i + 1].x() * weights[j + 1].y() * weights[k + 1].z();
						const int index = getInd(cell_idx.x() + i, cell_idx.y() + j, cell_idx.z() + k);
						grid[index] += w * moment_mass0;

						moment_mass0.head<3>() += kstep;
					}
					moment_mass0.head<3>() += jstep;
				}
				moment_mass0.head<3>() += istep;
			}
		}
	}

#ifdef TIME_COUNT_FLAG
	auto end = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	MSG("P2G " << elapsed << " us");
#endif


#ifdef TIME_COUNT_FLAG
	start = std::chrono::steady_clock::now();
#endif

	// Process grid
	for (unsigned int i = 0; i < grid_size; ++i)
	{
		for (unsigned int j = 0; j < grid_size; ++j)
		{
			for (unsigned int k = 0; k < grid_size; k++)
			{
				Eigen::Array4f& cell = grid[getInd(i, j, k)]; // REFERENCE
				// cell is (v.x(), v.y(), mass)
				// only if there is mass
				if (cell.w() > 0)
				{
					// normalize by mass
					// momentum to velocity
					cell /= cell.w();
					cell.head<3>() += dt * g;

					//float x = static_cast<float>(i) / height;
					//float y = static_cast<float>(j) / width;
					// resolution and borders
					//if (x < boundary || x > 1 - boundary || y > 1 - boundary)
					if (i < 2 && cell.x() < 0.0f)
					{
						cell.x() = 0.0f;
					}
					else if (i > grid_size - 3 && cell.x() > 0.0f)
					{
						cell.x() = 0.0f;
					}


					if (j < 2 && cell.y() < 0.0f)
					{
						cell.y() = 0.0f;
					}
					else if (j > grid_size - 3 && cell.y() > 0.0f)
					{
						cell.y() = 0.0f;
					}

					if (k < 2 && cell.z() < 0.0f)
					{
						cell.z() = 0.0f;
					}
					else if (k > grid_size - 3 && cell.z() > 0.0f)
					{
						cell.z() = 0.0f;
					}
				}
			}
		}
	}


#ifdef TIME_COUNT_FLAG
	end = std::chrono::steady_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	MSG("GRID P. " << elapsed << " us");

	start = std::chrono::steady_clock::now();
#endif

#if defined(TIME_COUNT_FLAG) && defined(G2P_FLAG)
	long long v1 = 0;
	long long v2 = 0;
	long long v3 = 0;
	long long v4 = 0;

#pragma omp parallel for reduction(+:v1,v2,v3,v4)
#else
#pragma omp parallel for
#endif

	// Grid to particle
	for (int i = 0; i < particles.size(); ++i)
	{
		Particle& p = particles[i];
#if defined(TIME_COUNT_FLAG) && defined(G2P_FLAG)
		auto start_in = std::chrono::steady_clock::now();
#endif
		// compute the center 
		const Eigen::Array3f& cell_idxf = (p.pos * grid_size);
		const Eigen::Array3i cell_idx = cell_idxf.cast<int>(); // floor
		const Eigen::Array3f distFromCenter = ((p.pos * grid_size) - cell_idx.cast<float>()) - 0.5f; // center at point 0,0

		const Eigen::Array3f weights[3] = {
		  0.5f * (0.5f - distFromCenter).square(),
		  0.75f - (distFromCenter).square(),
		  0.5f * (distFromCenter + 0.5f).square()
		};

		p.C.setZero();
		p.v.setZero();

#if defined(TIME_COUNT_FLAG) && defined(G2P_FLAG)
		end = std::chrono::steady_clock::now();
		v1 += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_in).count();

		start_in = std::chrono::steady_clock::now();
#endif
		// b-spline
		for (int i = -1; i < 2; ++i)
		{
			for (int j = -1; j < 2; ++j)
			{
				for (int k = 0; k < 2; k++)
				{
					const Eigen::Array3i cell_x = cell_idx + Eigen::Array3i(i, j, k);
					const Eigen::Vector3f cell_dist = (cell_x.cast<float>() - (p.pos * grid_size)) + 0.5f;

					const float w = weights[i + 1].x() * weights[j + 1].y();
					const Eigen::Array3f& cell_v = grid[getInd(cell_x.x(), cell_x.y(), cell_x.z())].head<3>();

					p.v += w * cell_v;

					// apic, eq 10
					//TODO sum outer product to p.C
					utils::SumOuterProduct(p.C, w * cell_v, cell_dist);
				}
			}
		}

		p.C *= 4.0f * grid_size;

#if defined(TIME_COUNT_FLAG) && defined(G2P_FLAG)
		end = std::chrono::steady_clock::now();
		v2 += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_in).count();

		start_in = std::chrono::steady_clock::now();
#endif

		assert(	p.pos.x() > 0.0f && p.pos.x() < 1.0f && 
				p.pos.y() > 0.0f && p.pos.y() < 1.0f &&
				p.pos.z() > 0.0f && p.pos.z() < 1.0f);
		// advect particles
		p.pos += dt * p.v;

		assert(	p.pos.x() > 0.0f && p.pos.x() < 1.0f && 
				p.pos.y() > 0.0f && p.pos.y() < 1.0f &&
				p.pos.z() > 0.0f && p.pos.z() < 1.0f);


		// safety clamp!!
		//p.pos = maxBorder.min(minBorder.max(p.pos)); // clamp!!!!

		// update F gradient
		Eigen::Matrix3f F = (Eigen::Matrix3f::Identity() + (dt * p.C)) * p.F;
		svd.compute(F);
#if defined(TIME_COUNT_FLAG) && defined(G2P_FLAG)
		end = std::chrono::steady_clock::now();
		v3 += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_in).count();

		start_in = std::chrono::steady_clock::now();
#endif
		const Eigen::Matrix3f& svd_u = svd.matrixU();
		const Eigen::Matrix3f& svd_v = svd.matrixV();

		Eigen::Vector3f svd_e = svd.singularValues();

		// Snow paper elasticiy constrains
		for (int i = 0; i < 3; ++i) {
			svd_e[i] = glm::clamp(svd_e[i], 1.0f - 2.5e-2f, 1.0f + 7.5e-3f);
		}

		// avoid infinities and NaNs
		assert(_finite(F(0, 0)) && _finite(F(0, 1)) && _finite(F(1, 0)) & _finite(F(1, 1)));


		const float oldJ = F.determinant();
		F = svd_u * svd_e.asDiagonal() * svd_v.transpose();

		// to avoid a division by 0 in the future
		assert((-F(0, 0)) != F(1, 1) || (F(1, 0)) != (F(1, 0)));

		const float det = F.determinant();
		const float newJ = glm::clamp(p.J * oldJ / det, 0.6f, 20.0f);

		p.F = F;
		p.J = newJ;

#if defined(TIME_COUNT_FLAG) && defined(G2P_FLAG)
		end = std::chrono::steady_clock::now();
		v4 += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_in).count();
#endif
	}

#if defined(TIME_COUNT_FLAG) && defined(G2P_FLAG)
	double t = static_cast<double>(v1) / particles.size();
	TMSG("Weights  " << t << " ns");
	t = static_cast<double>(v2) / particles.size();
	TMSG("B-Spline " << t << " ns");
	t = static_cast<double>(v3) / particles.size();
	TMSG("Adve&SVD " << t << " ns");
	t = static_cast<double>(v4) / particles.size();
	TMSG("end " << t << " ns");
#endif

#ifdef TIME_COUNT_FLAG
	end = std::chrono::steady_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	MSG("G2P " << elapsed << " us");
	MSG("");
#endif
}


void Simulator_3D::addParticle(const glm::vec3& pos, const glm::vec3& v)
{
	Eigen::Array3f Epos(pos.x, pos.y, pos.z);
	Eigen::Array3f Ev(v.x, v.y, v.z);
	Particle p(Epos * d_size, Ev);
	particles.push_back(p);
}

void Simulator_3D::addParticleNormalized(const glm::vec3& pos, const glm::vec3& v)
{
	Eigen::Array3f Epos(pos.x, pos.y, pos.z);
	Eigen::Array3f Ev(v.x, v.y, v.z);
	Particle p(Epos, Ev);
	particles.push_back(p);
}

