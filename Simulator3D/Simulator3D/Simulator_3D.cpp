#include "Simulator_3D.h"

#include <algorithm>
#include <iostream>
#include <math.h>
//#define TIME_COUNT_FLAG
//#define G2P_FLAG


#ifdef TIME_COUNT_FLAG
#include <chrono>
#endif

Simulator_3D::Simulator_3D(HYPERELASTICITY mode) :
	grid_size(128), d_size(1.0f / grid_size), mode(mode)
{
	minBorder = Eigen::Array3f::Constant(0.0f + 1.0e-3f);
	maxBorder = Eigen::Array3f::Constant(1.0f - 1.0e-3f);


	//particles = std::vector<Particle>(0);
	grid = new Eigen::Array4f[((int)grid_size * (int)grid_size * (int)grid_size)];
	std::memset(grid, 0, (static_cast<size_t>(grid_size * grid_size * grid_size) * static_cast<size_t>(sizeof(Eigen::Array4f))));

	physicsGrid = new Eigen::Array3f[((int)grid_size * (int)grid_size * (int)grid_size)];
	clearPhysics();
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
	std::memset(grid, 0, (static_cast<int>(grid_size * grid_size * grid_size) * static_cast<int>(sizeof(Eigen::Array4f))));

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

		property& p_prop = v_properties[p.prop_id];
		// Lame parameters
		const float e = std::exp(p_prop.hardening * (1.0f - p.J));
		const float mu = p_prop.mu * e;
		const float lambda = p_prop.lambda * e;

		const float J = (p.F).determinant();
		// Euler explicit time integration
		// Looking for stress
		Eigen::Matrix3f affine;
		if (this->mode == HYPERELASTICITY::COROTATED)
		{
			Eigen::JacobiSVD<Eigen::Matrix3f, Eigen::NoQRPreconditioner> svd(p.F, Eigen::ComputeFullU | Eigen::ComputeFullV);

			const Eigen::Matrix3f r = svd.matrixU() * svd.matrixV().transpose();


			//Corotated constitucional model     // [http://mpm.graphics Eqn. 52]
			const Eigen::Matrix3f PF_t = (2.0f * mu * (p.F - r) * (p.F).transpose()) + (Eigen::Matrix3f::Identity() * (lambda * (J - 1.0f) * J));

			const float DinvSQ = (4.0f * grid_size * grid_size);
			//EQn. 173
			const Eigen::Matrix3f stress = (-dt * p_prop.volume * DinvSQ) * PF_t; // eq_16_term_0

			affine = stress + p_prop.mass * p.C;
		}
		else if (this->mode == HYPERELASTICITY::NEOHOOKEAN)
		{
			// Neo-hookean multiplyed by F^t
			const Eigen::Matrix3f PF_t = (mu * ((p.F * (p.F).transpose()) - Eigen::Matrix3f::Identity())) +
				(Eigen::Matrix3f::Identity() * (lambda * std::log(J)));
			const float DinvSQ = (4.0f * grid_size * grid_size);
			//EQn. 173
			const Eigen::Matrix3f stress = (-dt * p_prop.volume * DinvSQ) * PF_t; // eq_16_term_0

			affine = stress + p_prop.mass * p.C;
		}
		else
		{
			// SAND
			affine = p_prop.mass * p.C;
		}
		//P2G
#if true // optimization
		{
		const Eigen::Array3i cell_x0 = cell_idx + Eigen::Array3i::Constant(-1);
		const Eigen::Vector3f cell_dist0 = ((cell_x0.cast<float>() - (p.pos * grid_size)) + 0.5f);


			Eigen::Array4f moment_mass0 = (Eigen::Array4f() << p.v * p_prop.mass, p_prop.mass).finished(); // moment and particle mass
			moment_mass0.head<3>() += d_size * (affine * cell_dist0).array();

			const Eigen::Array3f kstep = affine.col(2) * d_size;
			const Eigen::Array3f jSemiStep = (affine.col(1) * d_size).array();
			const Eigen::Array3f jstep = jSemiStep - (3.0f * kstep);
			const Eigen::Array3f istep = (affine.col(0) * d_size).array() - (3.0f * jSemiStep);

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
#else
		{
			for (int i = -1; i < 2; ++i)
			{
				for (int j = -1; j < 2; ++j)
				{
					for (int k = -1; k < 2; ++k)
					{
						// cell_x is the idx of the selected cell to update
						const Eigen::Array3i cell_x = cell_idx + Eigen::Array3i(i, j, k);

						// cell_distance to the particle in the corresponding updating cell
						const Eigen::Vector3f cell_dist = d_size * ((cell_x.cast<float>() - (p.pos * grid_size)) + 0.5f);

						const float w = weights[i + 1].x() * weights[j + 1].y() * weights[k + 1].z();
						const int index = getInd(cell_x.x(), cell_x.y(), cell_x.z());

						//TODO: optimitzar multiplicant w nomes per mass
						Eigen::Array4f moment_mass = (Eigen::Array4f() << p.v * mass, mass).finished(); // moment and particle mass

						moment_mass.head<3>() += (affine * cell_dist).array();

						grid[index] += w * moment_mass;

					}
				}
			}
		}
#endif
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
				int idx = getInd(i, j, k);
				Eigen::Array4f& cell = grid[idx]; // REFERENCE
				// cell is (v.x(), v.y(), mass)
				// only if there is mass
				if (cell.w() > 0)
				{
					// normalize by mass
					// momentum to velocity
					cell /= cell.w();
					cell.head<3>() += dt * g;

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

					const Eigen::Vector3f& normalPhyisics = physicsGrid[idx];
					float dot = normalPhyisics.dot(cell.head<3>().matrix());
					if (dot < 0.0f)
					{
						cell.head<3>() = cell.head<3>() - (dot * normalPhyisics).array();
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
				for (int k = -1; k < 2; ++k)
				{
					const Eigen::Array3i cell_x = cell_idx + Eigen::Array3i(i, j, k);
					const Eigen::Vector3f cell_dist = (cell_x.cast<float>() - (p.pos * grid_size)) + 0.5f;

					const float w = weights[i + 1].x() * weights[j + 1].y() * weights[k + 1].z();
					const Eigen::Array3f& cell_v = grid[getInd(cell_x.x(), cell_x.y(), cell_x.z())].head<3>();

					p.v += w * cell_v;

					// apic, eq 10
					SumOuterProduct(p.C, w * cell_v, cell_dist);
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

		assert(p.pos.x() > 0.0f && p.pos.x() < 1.0f &&
			p.pos.y() > 0.0f && p.pos.y() < 1.0f &&
			p.pos.z() > 0.0f && p.pos.z() < 1.0f);

		// update F gradient (mls Eq. 17)
		Eigen::Matrix3f F = (Eigen::Matrix3f::Identity() + (dt * p.C)) * p.F;
		// avoid infinities and NaNs
		assert(std::isfinite(F(0, 0)) && std::isfinite(F(0, 1)) && std::isfinite(F(1, 0)) & std::isfinite(F(1, 1)));

		
		Eigen::JacobiSVD<Eigen::Matrix3f, Eigen::NoQRPreconditioner> svd(F, Eigen::ComputeFullU | Eigen::ComputeFullV);

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

		

		const float oldJ = F.determinant();
		F = svd_u * svd_e.asDiagonal() * svd_v.transpose();
		// avoid infinities and NaNs
		assert(std::isfinite(F(0, 0)) && std::isfinite(F(0, 1)) && std::isfinite(F(1, 0)) & std::isfinite(F(1, 1)));



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


void Simulator_3D::addParticle(const glm::vec3& pos, const glm::vec3& v, int material)
{
	Eigen::Array3f Epos(pos.x, pos.y, pos.z);
	Eigen::Array3f Ev(v.x, v.y, v.z);
	Particle p(Epos * d_size, Ev, material);
	particles.push_back(p);
}

void Simulator_3D::addParticleNormalized(const glm::vec3& pos, const glm::vec3& v, int material)
{
	Eigen::Array3f Epos(pos.x, pos.y, pos.z);
	Eigen::Array3f Ev(v.x, v.y, v.z);
	Particle p(Epos, Ev, material);
	particles.push_back(p);
}

void Simulator_3D::clearPhysics()
{
	std::memset(physicsGrid, 0, (static_cast<size_t>(grid_size * grid_size * grid_size)* static_cast<size_t>(sizeof(Eigen::Array3f))));
}

void Simulator_3D::setPhysicsFlat(float height)
{
	// convert height from 0-1 to 0-gridsize
	const int y = static_cast<int>(height * grid_size);

	for (int x = 0; x < grid_size; ++x)
	{
		for (int z = 0; z < grid_size; ++z)
		{
			physicsGrid[getInd(x, y, z)] = Eigen::Array3f(0.0f, 1.0f, 0.0f);
		}
	}
}

void Simulator_3D::setPhysicSlopes(float height, float initialH, float holeWidth, int depth)
{
	const int desp = static_cast<int>(height * grid_size);
	const int w = static_cast<int>(holeWidth * grid_size);

	const int y_max = static_cast<int>(grid_size * initialH) + desp;
	assert(y_max < grid_size - 1 && w < grid_size * 0.5f - 1);

	Eigen::Vector3f normal;
	Eigen::Vector3f pointInPlane;
	{
		Eigen::Vector3f p0(.0f, height * grid_size, .0f);
		Eigen::Vector3f p1(grid_size * 0.5f - holeWidth * grid_size, .0f, .0f);
		Eigen::Vector3f slope = p0 - p1;

		const Eigen::Vector3f tmp(.0f, .0f, -1.0f);
		normal = tmp.cross(slope);
		normal.normalize();

		pointInPlane = p0;
		pointInPlane.y() += grid_size * initialH;
	}

	auto belowPlane = [&normal, &pointInPlane](Eigen::Vector3f point)
	{
		Eigen::Vector3f plan2point = point - pointInPlane;
		float dot = plan2point.dot(normal);

		return dot < 0;
	};

	for (int y = static_cast<int>(grid_size * initialH - 1); y < y_max; ++y)
	{
		for (int x = 0; x < static_cast<int>(grid_size * 0.5f - w); ++x)
		{
			for (int z = 0; z < static_cast<int>(grid_size); ++z)
			{
				Eigen::Vector3f p(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
				p += Eigen::Vector3f::Constant(0.5f);
				if (belowPlane(p))
				{
					p += depth * normal;
					p = p.cwiseMin(grid_size - 1);
					if (!belowPlane(p))
					{
						physicsGrid[getInd(x, y, z)] = normal.array();
					}
				}
			}
		}
	}

	{
		Eigen::Vector3f p0(grid_size - 1, height * grid_size, .0f);
		Eigen::Vector3f p1(grid_size * 0.5f + holeWidth * grid_size, .0f, .0f);
		Eigen::Vector3f slope = p1 - p0;

		const Eigen::Vector3f tmp(.0f, .0f, -1.0f);
		normal = tmp.cross(slope);
		normal.normalize();

		pointInPlane = p0;
		pointInPlane.y() += grid_size * initialH;

	}

	for (int y = static_cast<int>(grid_size * initialH - 1); y < y_max; ++y)
	{
		for (int x = static_cast<int>(grid_size * 0.5f + w); x < static_cast<int>(grid_size); ++x)
		{
			for (int z = 0; z < static_cast<int>(grid_size); ++z)
			{
				Eigen::Vector3f p(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
				p += Eigen::Vector3f::Constant(0.5f);
				if (belowPlane(p))
				{
					p += depth * normal;
					p = p.cwiseMin(grid_size - 1);
					if (!belowPlane(p))
					{
						physicsGrid[getInd(x, y, z)] = normal.array();
					}
				}
			}
		}
	}
}

void Simulator_3D::setPhysicsZWall(float zmin, float zmax, int depth)
{
	assert(zmin > 0 && zmax < grid_size && zmin < zmax);

	int z_min = static_cast<int>(grid_size * zmin);
	int z_max = static_cast<int>(grid_size * zmax);
	for (int d = 0; d < depth; ++d)
	{
		for (int y = 0; y < grid_size; ++y)
		{
			for (int x = 0; x < grid_size; ++x)
			{
				physicsGrid[getInd(x, y, z_min - d)] = Eigen::Array3f(.0f, .0f, 1.0f);
				physicsGrid[getInd(x, y, z_max + d)] = Eigen::Array3f(.0f, .0f, -1.0f);

			}
		}
	}
}

int Simulator_3D::addNewMaterial(float young, float nu, float hardening, float volume, float mass)
{
	property prop(young, nu, hardening, volume, mass);
	v_properties.push_back(prop);

	return static_cast<int>(v_properties.size()) - 1;
}

void Simulator_3D::SumOuterProduct(Eigen::Matrix3f& r, const Eigen::Array3f& a, const Eigen::Array3f& b)
{

	r(0, 0) += a[0] * b[0];
	r(0, 1) += a[0] * b[1];
	r(0, 2) += a[0] * b[2];

	r(1, 0) += a[1] * b[0];
	r(1, 1) += a[1] * b[1];
	r(1, 2) += a[1] * b[2];

	r(2, 0) += a[2] * b[0];
	r(2, 1) += a[2] * b[1];
	r(2, 2) += a[2] * b[2];
}