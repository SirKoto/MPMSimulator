#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cstdint>
#include <stdio.h>
#include <vector>
#include "Simulator_3D_GPU.h"
#define GENERATE_IMPLEMENTATION_MM
#include "MinMath.h"

#include "svd3_cuda.cuh"

using namespace mm;

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char* file, int line, bool abort = true)
{
	if (code != cudaSuccess)
	{
		fprintf(stderr, "GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
		if (abort) exit(code);
	}
}

struct ParticleGPU
{
	vec3 pos, v; // position and velocity

	mat3 F, C; // Deformation gradient and C from APIC

	float Jp; // Determinat of plastic F (Jacobian).

	const int prop_id; // material id
};

extern "C"
void loadParticles2GPU(const S3::Particle * particles, size_t num);
extern "C"
void loadGrid2GPU(size_t num);
extern "C"
void loadPhysicsGrid2GPU(const Eigen::Array3f * grid, size_t num);
extern "C"
void loadProperties2GPU(const S3::property * properties, size_t num);
extern "C"
void storeParticles2Array(S3::Particle * particles, size_t num);
extern "C"
void runIterationGPU(float dt);

__device__ ParticleGPU* d_particles;
__device__ __constant__ unsigned int d_numParticles;
unsigned int h_numParticles;
__device__ vec4* d_grid;
vec4* h_grid;
__device__ __constant__ vec3* d_physicsGrid;
__device__ __constant__ S3::property* d_properties;

#define GRID_SIZE 128.0f
#define GRID_SIZE_I 128
#define D_SIZE (1.0f/128.0f)
#define G vec3{0.0f, -10.0f, 0.0f}
#define getInd(x, y, z) (((((x) << 7) | (y)) << 7) | (z))

#define BLOCK_SIZE 32

__global__ void P2G_NeoHookean(const float dt) {
unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

if (idx < d_numParticles) {
	const ParticleGPU& p = d_particles[idx];
	// Discretize position 
	const vec3 cell_if = mm::mul(p.pos, GRID_SIZE);
	int cell_x = cell_if.x, cell_y = cell_if.y, cell_z = cell_if.z; // floor
	// Vector from cell center -> particle
	vec3 distFromCenter = mm::sub(cell_if, mm::floor(cell_if));
	mm::add_to(&distFromCenter, -0.5f); // center at point 0,0,0


	// Interpolation function N matrix
	vec3 weights[3];
	{
		{
			vec3 center = mm::sub(0.5f, distFromCenter);
			mm::square_in(&center); // Squared
			weights[0] = mm::mul(center, 0.5f);
		}
		{
			vec3 center = distFromCenter;
			mm::square_in(&center); // Squared
			weights[1] = mm::sub(0.75f, center);
		}
		{
			vec3 center = mm::add(distFromCenter, 0.5f);
			mm::square_in(&center); // Squared
			weights[2] = mm::mul(center, 0.5f);
		}
	}

	// Load material properies
	const S3::property& p_prop = d_properties[p.prop_id];

	// Lame parameters
	const float e = expf(p_prop.hardening * (1.0f - p.Jp));
	const float mu = p_prop.mu * e;
	const float lambda = p_prop.lambda * e;

	const float mass = p_prop.mass;

	const float J = mm::determinant(p.F);

	// ----------  AFFINE MATRIX ---------- //
	mat3 affine = mm::mul(p.C, mass);
	if(true){
		mat3 PF_t = mm::mul_trans(p.F);
		mm::add_to(&PF_t, -1.0f); // Add only to diagonal
		mm::mul_in(&PF_t, mu); // Multiply by mu
		mm::add_to(&PF_t, lambda * logf(J));

		const float Dinv = (4.0f * GRID_SIZE * GRID_SIZE);
		const float K = -dt * p_prop.volume * Dinv;
		mm::mul_in(&PF_t, K); // Compute stress
		//const Eigen::Matrix3f stress = (-dt * p_prop.volume * Dinv) * PF_t;
		mm::add_to(&affine, PF_t);
	}

	// ----------  PARTICLE TRANSFERENCE ---------- //
	// This can be improved by using the previously computed distFromCenter
	vec3 cell_dist0 = mm::add(mm::floor(cell_if), -1.0f);
	cell_dist0 = mm::sub(cell_dist0, cell_if);
	mm::add_to(&cell_dist0, 0.5f);

	vec3 moment = mm::mul(p.v, mass);

	vec4 moment_mass0 = { moment.x, moment.y, moment.z, mass }; // moment and particle mass
	
	vec3 contribution = mm::mul(affine, cell_dist0);
	mm::mul_in(&contribution, D_SIZE);
	mm::add_to(&moment_mass0, contribution); // Add contribution to momentum of the first cell

	const vec3 kstep = mm::mul(mm::col2(affine), D_SIZE);// affine.col(2)* D_SIZE;
	const vec3 jSemiStep = mm::mul(mm::col1(affine), D_SIZE); //(affine.col(1) * D_SIZE).array();
	const vec3 jstep = mm::sub(jSemiStep, mm::mul(kstep, 3.0f));//jSemiStep -(3.0f * kstep);
	const vec3 iSemiStep = mm::mul(mm::col0(affine), D_SIZE);
	const vec3 istep = mm::sub(iSemiStep, mm::mul(jSemiStep, 3.0f)); //(affine.col(0) * D_SIZE).array() - (3.0f * jSemiStep);

	float w;
	unsigned int index;
	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			for (int k = -1; k < 2; ++k)
			{
				w = weights[i + 1].x * weights[j + 1].y * weights[k + 1].z;
				vec4 value = mm::mul(moment_mass0, w);
				index = getInd(cell_x + i, cell_y + j, cell_z + k);
				vec4* ptr = d_grid + index;

				atomicAdd(reinterpret_cast<float*>(&((*ptr).x)), value.x);
				atomicAdd(reinterpret_cast<float*>(&((*ptr).y)), value.y);
				atomicAdd(reinterpret_cast<float*>(&((*ptr).z)), value.z);
				atomicAdd(reinterpret_cast<float*>(&((*ptr).w)), value.w);

				mm::add_to(&moment_mass0, kstep);
			}
			mm::add_to(&moment_mass0, jstep);
		}
		mm::add_to(&moment_mass0, istep);
	}
}
}


__global__ void GridProcessing(const float dt) {
	unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned int k = idx % GRID_SIZE_I;
	unsigned int j = (idx / GRID_SIZE_I) % GRID_SIZE_I;
	unsigned int i = (idx / (GRID_SIZE_I * GRID_SIZE_I)) % GRID_SIZE_I;

	vec4 cell = d_grid[idx]; // reference


	if (cell.w > 0.0f)
	{
		// ----------  MOMENTUM 2 VELOCITY---------- //
		cell.x /= cell.w;
		cell.y /= cell.w;
		cell.z /= cell.w;

		// Gravity
		mm::add_to(&cell, mm::mul(G, dt));
		// ----------  LIMITS ---------- //
		if (i < 2 && cell.x < 0.0f)
		{
			cell.x = 0.0f;
		}
		else if (i > GRID_SIZE - 3 && cell.x > 0.0f)
		{
			cell.x = 0.0f;
		}


		if (j < 2 && cell.y < 0.0f)
		{
			cell.y = 0.0f;
		}
		else if (j > GRID_SIZE - 3 && cell.y > 0.0f)
		{
			cell.y = 0.0f;
		}

		if (k < 2 && cell.z < 0.0f)
		{
			cell.z = 0.0f;
		}
		else if (k > GRID_SIZE - 3 && cell.z > 0.0f)
		{
			cell.z = 0.0f;
		}

		// ----------  PHYSICS ---------- //
		const vec3& normalPhyisics = d_physicsGrid[idx];
		// velocity dot normal
		float dot = mm::dot3(cell, normalPhyisics);
		// If oposed
		if (dot < 0.0f)
		{
			assert(false);
			// Remove normal velocity
			vec3 r = mm::mul(normalPhyisics, -dot);
			mm::add_to(&cell, r);
		}

		d_grid[idx] = cell;
	}
}




__global__ void G2P(const float dt) {
unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;
if (idx < d_numParticles) {
	ParticleGPU& p = d_particles[idx];
	
	// compute the center 
	const vec3 cell_if = mm::mul(p.pos, GRID_SIZE);
	// Vector from cell center -> particle
	vec3 distFromCenter = mm::sub(cell_if, mm::floor(cell_if));
	mm::add_to(&distFromCenter, -0.5f); // center at point 0,0,0

	// Interpolation function N matrix
	vec3 weights[3];
	{
		{
			vec3 center = mm::sub(0.5f, distFromCenter);
			mm::square_in(&center); // Squared
			weights[0] = mm::mul(center, 0.5f);
		}
		{
			vec3 center = distFromCenter;
			mm::square_in(&center); // Squared
			weights[1] = mm::sub(0.75f, center);
		}
		{
			vec3 center = mm::add(distFromCenter, 0.5f);
			mm::square_in(&center); // Squared
			weights[2] = mm::mul(center, 0.5f);
		}
	}

	// Set to zero velocity and C, befor transfer
	mm::set_zero(&p.C);
	mm::set_zero(&p.v);

	// ----------  GRID TRANSFER ---------- //
	// First cell
	int cell_x = cell_if.x, cell_y = cell_if.y, cell_z = cell_if.z;
	cell_x -= 1; cell_y -= 1; cell_z -= 1;
	// vector particle -> center cell
	vec3 cell_dist = mm::sub(mm::floor(cell_if), cell_if);
	mm::add_to(&cell_dist, -0.5f); // -1 + 0.5
	float w;
	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			for (int k = -1; k < 2; ++k)
			{
				w = weights[i + 1].x * weights[j + 1].y * weights[k + 1].z;
				const vec3 cell_v = d_grid[getInd(cell_x, cell_y, cell_z)];

				vec3 contri = mm::mul(cell_v, w);
				mm::add_to(&p.v, contri);

				mm::add_outter_product(&p.C, contri, cell_dist);

				cell_z += 1;
				cell_dist.z += 1.0f;
			}
			cell_z -= 3;
			cell_y += 1;

			cell_dist.z -= 3.0f;
			cell_dist.y += 1.0f;
		}
		cell_y -= 3;
		cell_x += 1;

		cell_dist.y -= 3.0f;
		cell_dist.x += 1.0f;
	}
	// Apply D^-1 to get C
	mm::mul_in(&p.C, 4.0f * GRID_SIZE);

	// ----------  ADVECTION ---------- //
	//Eigen::Array3f tmp = p.pos;
	mm::add_to(&p.pos, mm::mul(p.v, dt));
	//printf("Pos  %f, %f, %f, %f\n", p.pos.x, p.pos.y, p.pos.z, dt);

	//printf("Pos2 %i, %i, %i\n", tmp.x() != p.pos.x(), tmp.y() != p.pos.y(), tmp.z() != p.pos.z());
	// Assert that the position is correct!!
	assert(p.pos.x >= 0.0f && p.pos.x <= 1.0f &&
		p.pos.y >= 0.0f && p.pos.y <= 1.0f &&
		p.pos.z >= 0.0f && p.pos.z <= 1.0f);

	// ----------  DEFORMATION GRADIENT UPDATE ---------- //
	mat3 Ftmp = mm::mul(p.C, dt);
	mm::add_to(&Ftmp, 1.0f);
	mat3 F = mm::mul(Ftmp, p.F);

	const S3::property& p_prop = d_properties[p.prop_id];
	if (p_prop.plasticity)
	{
		mat3 u, v;
		vec3 s;

		svd(F.m[0][0], F.m[0][1], F.m[0][2],
			F.m[1][0], F.m[1][1], F.m[1][2],
			F.m[2][0], F.m[2][1], F.m[2][2],
			u.m[0][0], u.m[0][1], u.m[0][2],
			u.m[1][0], u.m[1][1], u.m[1][2],
			u.m[2][0], u.m[2][1], u.m[2][2],
			s.x, s.y, s.z,
			v.m[0][0], v.m[0][1], v.m[0][2],
			v.m[1][0], v.m[1][1], v.m[1][2],
			v.m[2][0], v.m[2][1], v.m[2][2]);

		// Snow paper elasticiy constrains
		s.x = glm::clamp(s.x, 1.0f - p_prop.t_c, 1.0f + p_prop.t_s);
		s.y = glm::clamp(s.y, 1.0f - p_prop.t_c, 1.0f + p_prop.t_s);
		s.z = glm::clamp(s.z, 1.0f - p_prop.t_c, 1.0f + p_prop.t_s);

		mat3 S;
		S.m[0][0] = s.x;
		S.m[1][1] = s.y;
		S.m[2][2] = s.z;

		const float oldJ = mm::determinant(F);

		F = mm::mul(u, mm::mul_trans(S, v));

		const float det = mm::determinant(F);
		const float newJ = glm::clamp(p.Jp * oldJ / det, p_prop.p_c, p_prop.p_s);

		p.Jp = newJ;
	}

	/*
	// ----------  PLASTICITY ---------- //
	// SVD of new deformation gradient F
	Eigen::JacobiSVD<Eigen::Matrix3f, Eigen::NoQRPreconditioner> svd(F, Eigen::ComputeFullU | Eigen::ComputeFullV);

	const S3::property& prop = d_properties[p.prop_id];
	if (prop.plasticity) {
		const Eigen::Matrix3f& svd_u = svd.matrixU();
		const Eigen::Matrix3f& svd_v = svd.matrixV();

		Eigen::Vector3f svd_e = svd.singularValues();

		// Snow paper elasticiy constrains
		for (int i = 0; i < 3; ++i) {
			svd_e[i] = glm::clamp(svd_e[i], 1.0f - prop.t_c, 1.0f + prop.t_s);
		}

		const float oldJ = F.determinant();
		F = svd_u * svd_e.asDiagonal() * svd_v.transpose();

		const float det = F.determinant();
		const float newJ = glm::clamp(p.Jp * oldJ / det, prop.p_c, prop.p_s);
		p.Jp = newJ;
	}*/

	p.F = F;

}
}

void loadParticles2GPU(const S3::Particle* particles, size_t num)
{
	static_assert(sizeof(S3::Particle) == sizeof(ParticleGPU),"Particle GPU must be equivalent to CPU one");
	static_assert(offsetof(S3::Particle, F) == offsetof(ParticleGPU, F), "Offsets not equivalent");
	static_assert(offsetof(S3::Particle, v) == offsetof(ParticleGPU, v), "Offsets not equivalent");
	static_assert(offsetof(S3::Particle, C) == offsetof(ParticleGPU, C), "Offsets not equivalent");
	static_assert(offsetof(S3::Particle, Jp) == offsetof(ParticleGPU, Jp), "Offsets not equivalent");


	ParticleGPU* tmp;
	gpuErrchk(cudaMalloc((ParticleGPU**)&tmp, num * sizeof(ParticleGPU)));
	gpuErrchk(cudaMemcpy(tmp, particles, num * sizeof(ParticleGPU), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpyToSymbol(d_particles, (void**)&tmp, sizeof(ParticleGPU*)));

	h_numParticles = static_cast<unsigned int>(num);
	gpuErrchk(cudaMemcpyToSymbol(d_numParticles, &h_numParticles, sizeof(unsigned int)));

}

void loadGrid2GPU(size_t num)
{
	assert(static_cast<size_t>(GRID_SIZE) == num);

	num = num * num * num;
	gpuErrchk(cudaMalloc((vec4**)& h_grid, num * sizeof(vec4)));
	gpuErrchk(cudaMemset(h_grid, 0, num * sizeof(vec4)));
	gpuErrchk(cudaMemcpyToSymbol(d_grid, (void**)&h_grid, sizeof(vec4*)));
}

void loadPhysicsGrid2GPU(const Eigen::Array3f* grid, size_t num)
{
	assert(static_cast<size_t>(GRID_SIZE) == num);

	num = num * num * num;
	vec3* tmp;
	gpuErrchk(cudaMalloc((vec3**) & tmp, num * sizeof(vec3)));
	gpuErrchk(cudaMemcpy(tmp, grid, num * sizeof(vec3), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpyToSymbol(d_physicsGrid, (void**)&tmp, sizeof(vec3*)));
}

void loadProperties2GPU(const S3::property* properties, size_t num)
{
	S3::property* tmp;
	gpuErrchk(cudaMalloc((S3::property**) & tmp, num * sizeof(S3::property)));
	gpuErrchk(cudaMemcpy(tmp, properties, num * sizeof(S3::property), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpyToSymbol(d_properties, (void**)&tmp, sizeof(S3::property*)));
}

void storeParticles2Array(S3::Particle* particles, size_t num)
{
	S3::Particle* tmp;
	gpuErrchk(cudaMemcpyFromSymbol(&tmp,d_particles, sizeof(ParticleGPU*)));
	gpuErrchk(cudaMemcpy(particles, tmp, num * sizeof(ParticleGPU), cudaMemcpyDeviceToHost));
}

void clearGridGPU()
{
	size_t num = GRID_SIZE_I * GRID_SIZE_I * GRID_SIZE_I;
	gpuErrchk(cudaMemset(h_grid, 0, num * sizeof(vec4)));
}

void runIterationGPU(float dt)
{
	clearGridGPU();
	gpuErrchk(cudaDeviceSynchronize());// Coment this on release

	unsigned int numBlocks = (h_numParticles-1) / BLOCK_SIZE + 1;
	P2G_NeoHookean <<<numBlocks, BLOCK_SIZE >>> (dt);
	gpuErrchk(cudaPeekAtLastError());
	gpuErrchk(cudaDeviceSynchronize());// Coment this on release


	int nBlocks = GRID_SIZE_I * GRID_SIZE_I * GRID_SIZE_I / BLOCK_SIZE;

	GridProcessing <<<nBlocks, BLOCK_SIZE >>> (dt);
	gpuErrchk(cudaPeekAtLastError());
	gpuErrchk(cudaDeviceSynchronize());// Coment this on release

	G2P <<<numBlocks, BLOCK_SIZE >>> (dt);
	gpuErrchk(cudaPeekAtLastError());
	gpuErrchk(cudaDeviceSynchronize());// Coment this on release
}