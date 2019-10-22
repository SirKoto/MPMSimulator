#include "Simulator_2D.h"

#include "Utils.h"

#include <algorithm>
#include <iostream>

//#define TIME_COUNT_FLAG
//#define G2P_FLAG


#ifdef TIME_COUNT_FLAG
#include <chrono>
#endif

Simulator_2D::Simulator_2D(float E, float nu) :
	mu_0(E / (2 * (1 + nu))),
	lambda_0(E * nu / ((1 + nu) * (1 - 2 * nu))),
	width(80),
	height(80)
{ 
	grid_size = 128;

	minBorder = Eigen::Array2f::Constant(0.0f + 1.0e-3);
	maxBorder = Eigen::Array2f::Constant(1.0f - 1.0e-3);

	aspectR = static_cast<float>(width)/ height;

	d_size = 1.0f / grid_size;

	//particles = std::vector<Particle>(0);
	std::memset(grid, 0, sizeof(grid));
}

unsigned int Simulator_2D::dumpPositions(float* positions) const
{
	unsigned int i = 0;
	const unsigned int size = static_cast<unsigned int>(particles.size()) << 1;
	for (; i < size; i += 2)
	{
		const int j = i >> 1;
		positions[i] = particles[j].pos.x() * grid_size;
		positions[i + 1] = particles[j].pos.y() * grid_size;
	}
	return size >> 1;
}

unsigned int Simulator_2D::dumpPositionsNormalized(float* positions) const
{
	assert(false);

	unsigned int i = 0;
	const unsigned int size = static_cast<unsigned int>(particles.size()) >> 1; // div 2
	for (; i < size; ++i)
	{
		// TODO: posar que en un sol pas copii tot (ja que un vec2 en teoria son els dos floats seguits)
		positions[2 * i] = particles[i].pos.x() * d_size;// *grid_size.x();
		positions[2 * i + 1] = particles[i].pos.y() * d_size;// *grid_size.y();
	}
	return size;
}

void Simulator_2D::step(float dt)
{
	// all grid with 0's, velocity and mass
	std::memset(grid, 0, sizeof(grid));

#ifdef TIME_COUNT_FLAG
	auto start = std::chrono::steady_clock::now();
#endif
	// Particle to grid
	for (auto& p : this->particles)
	{
		assert(p.pos.x() > 0.0f && p.pos.x() < 1.0f && p.pos.y() > 0.0f && p.pos.y() < 1.0f);
		// compute the center 
		const Eigen::Array2f& cell_idxf = (p.pos * grid_size);
		const Eigen::Array2i cell_idx = cell_idxf.cast<int>(); // floor
		const Eigen::Array2f distFromCenter = ((p.pos * grid_size) - cell_idx.cast<float>()) - 0.5f; // center at point 0,0

		const Eigen::Array2f weights[3] = {
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


		Eigen::Matrix2f s, r;
		utils::polarDecomposition2D(p.F, s, r); // Decompose the deformation gradient in a rotation and an scale


		//Corotated constitucional model     // [http://mpm.graphics Eqn. 52]
		const Eigen::Matrix2f PF = 2.0f * mu * (p.F - r) * (p.F).transpose() + (Eigen::Matrix2f::Identity() * (lambda * (J - 1.0f) * J));

		//const glm::vec2 Dinv = 4.0f * grid_size * grid_size;
		// Identity sclae by inverse derivate
		//const glm::mat2 DinvM = glm::mat2(Dinv.x(), 0.0f, 0.0f, Dinv.y());
		//EQn. 173
		const Eigen::Matrix2f stress = - (dt * volume * 4.0f * grid_size * grid_size * PF); // eq_16_term_0

		const Eigen::Matrix2f affine = stress + mass * p.C;

		//P2G
		const Eigen::Array2i cell_x0 = cell_idx + Eigen::Array2i(-1, -1);
		const Eigen::Vector2f cell_dist0 =  ((cell_x0.cast<float>() - (p.pos * grid_size)) + 0.5f);

		{
			Eigen::Array3f moment_mass0 = (Eigen::Array3f() << p.v * mass, mass).finished(); // moment and particle mass
			moment_mass0.head<2>() += d_size * (affine * cell_dist0).array();

			const Eigen::Array2f jstep =  affine.col(1) * d_size;
			const Eigen::Array2f istep = (affine.col(0) * d_size).array() - (3.0f * jstep);

			for (int i = -1; i < 2; ++i)
			{
				for (int j = -1; j < 2; ++j)
				{

					const float w = weights[i + 1].x() * weights[j + 1].y();
					grid[getInd(cell_idx.x() + i, cell_idx.y() + j)] += w * moment_mass0;



					moment_mass0.head<2>() += jstep;
				}
				moment_mass0.head<2>() += istep;
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
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			Eigen::Array3f& cell = grid[getInd(i,j)]; // REFERENCE
			// cell is (v.x(), v.y(), mass)
			// only if there is mass
			if (cell.z() > 0)
			{
				// normalize by mass
				// momentum to velocity
				cell /= cell.z();
				cell.head<2>() += dt * g;

				//float x = static_cast<float>(i) / height;
				//float y = static_cast<float>(j) / width;
				// resolution and borders
				//if (x < boundary || x > 1 - boundary || y > 1 - boundary)
				if(i < 2 && cell.x() < 0.0f)
				{
					cell.x() = 0.0f;
				}
				else if (i > height - 3 && cell.x() > 0.0f)
				{
					cell.x() = 0.0f;
				}


				if (j < 2 && cell.y() < 0.0f)
				{
					cell.y() = 0.0f;
				}
				else if (j > width - 3 && cell.y() > 0.0f)
				{
					cell.y() = 0.0f;
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
		const Eigen::Array2f& cell_idxf = (p.pos  * grid_size);
		const Eigen::Array2i cell_idx = cell_idxf.cast<int>(); // floor
		const Eigen::Array2f distFromCenter = ((p.pos * grid_size) - cell_idx.cast<float>()) - 0.5f; // center at point 0,0

		const Eigen::Array2f weights[3] = {
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

				const Eigen::Array2i cell_x = cell_idx + Eigen::Array2i(i, j);
				const Eigen::Vector2f cell_dist = (cell_x.cast<float>() - (p.pos * grid_size)) + 0.5f;
				//if (cell_x.x() < 0 || cell_x.y() < 0 || cell_x.x() > height || cell_x.y() > width) continue;

				const float w = weights[i + 1].x() * weights[j + 1].y();
				const Eigen::Array2f& cell_v = grid[getInd(cell_x.x(), cell_x.y())].head<2>();

				p.v += w * cell_v;

				// apic, eq 10
				// sum outer product to p.C
				utils::SumOuterProduct(p.C, w * cell_v, cell_dist);
			}
		}

		p.C *= 4.0f * grid_size;

#if defined(TIME_COUNT_FLAG) && defined(G2P_FLAG)
		end = std::chrono::steady_clock::now();
		v2 += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_in).count();

		start_in = std::chrono::steady_clock::now();
#endif

		assert(p.pos.x() > 0.0f && p.pos.x() < 1.0f && p.pos.y() > 0.0f && p.pos.y() < 1.0f);
		// advect particles
		p.pos += dt * p.v;
		assert(p.pos.x() > 0.0f && p.pos.x() < 1.0f && p.pos.y() > 0.0f && p.pos.y() < 1.0f);


		// safety clamp!!
		//p.pos = maxBorder.min(minBorder.max(p.pos)); // clamp!!!!

		// update F gradient
		Eigen::Matrix2f F = (Eigen::Matrix2f::Identity() + (dt * p.C)) * p.F;

		const Eigen::JacobiSVD<Eigen::Matrix2f, Eigen::NoQRPreconditioner> svd(F, Eigen::ComputeFullU | Eigen::ComputeFullV);
#if defined(TIME_COUNT_FLAG) && defined(G2P_FLAG)
		end = std::chrono::steady_clock::now();
		v3 += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_in).count();

		start_in = std::chrono::steady_clock::now();
#endif
		const Eigen::Matrix2f& svd_u = svd.matrixU();
		const Eigen::Matrix2f& svd_v = svd.matrixV();

		Eigen::Vector2f svd_e = svd.singularValues();

		// Snow paper elasticiy constrains
		for (int i = 0; i < 2; ++i) {
			svd_e[i] = glm::clamp(svd_e[i], 1.0f - 2.5e-2f, 1.0f + 7.5e-3f);

		}

		// avoid infinities and NaNs
		assert(_finite(F(0, 0)) && _finite(F(0, 1)) && _finite(F(1, 0)) & _finite(F(1, 1)));
		

		const float oldJ = F.determinant();
		F = svd_u * svd_e.asDiagonal() * svd_v.transpose();

		// to avoid a division by 0 in the future
		assert((-F(0, 0)) != F(1, 1) || (F(1, 0)) != (F(1, 0)));
		

		const float newJ = glm::clamp(p.J * oldJ / F.determinant(), 0.6f, 20.0f);

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


void Simulator_2D::addParticle(const glm::vec2& pos, const glm::vec2& v)
{
	Eigen::Array2f Epos(pos.x, pos.y);
	Eigen::Array2f Ev(0, 0);
	Particle p(Epos * d_size, Ev);
	particles.push_back(p);
}

