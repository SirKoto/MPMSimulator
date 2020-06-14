#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cstdint>
#include <stdio.h>
#include <vector>
#include "Simulator_3D_GPU.h"

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char* file, int line, bool abort = true)
{
	if (code != cudaSuccess)
	{
		fprintf(stderr, "GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
		if (abort) exit(code);
	}
}

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

__device__ S3::Particle* d_particles;
__device__ Eigen::Array4f* d_grid;
__device__ __constant__ Eigen::Array3f* d_physicsGrid;
__device__ __constant__ S3::property* d_properties;

#define GRID_SIZE 128.0f
#define D_SIZE = 1.0f/128.0f
#define G = Eigen::Array3f(0.0f, -10.0f, 0.0f);
#define getInd(x, y, z) (((((x) << 7) | (y)) << 7) | (z))

void loadParticles2GPU(const S3::Particle* particles, size_t num)
{
	S3::Particle* tmp;
	gpuErrchk(cudaMalloc((S3::Particle**)&tmp, num * sizeof(S3::Particle)));
	gpuErrchk(cudaMemcpy(tmp, particles, num * sizeof(S3::Particle), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpyToSymbol(d_particles, (void**)&tmp, sizeof(S3::Particle*)));
}

void loadGrid2GPU(size_t num)
{
	assert(static_cast<size_t>(GRID_SIZE) == num);

	num = num * num * num;
	Eigen::Array4f* tmp;
	gpuErrchk(cudaMalloc((Eigen::Array4f**)&tmp, num * sizeof(Eigen::Array4f)));
	gpuErrchk(cudaMemset((Eigen::Array4f**) & tmp, 0, num * sizeof(Eigen::Array4f)));
	gpuErrchk(cudaMemcpyToSymbol(d_grid, (void**)&tmp, sizeof(Eigen::Array4f*)));
}

void loadPhysicsGrid2GPU(const Eigen::Array3f* grid, size_t num)
{
	assert(static_cast<size_t>(GRID_SIZE) == num);

	num = num * num * num;
	Eigen::Array3f* tmp;
	gpuErrchk(cudaMalloc((Eigen::Array3f**) & tmp, num * sizeof(Eigen::Array3f)));
	gpuErrchk(cudaMemcpy(tmp, grid, num * sizeof(Eigen::Array3f), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpyToSymbol(d_physicsGrid, (void**)&tmp, sizeof(Eigen::Array3f*)));
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
	gpuErrchk(cudaMemcpyFromSymbol(&tmp,d_particles, num * sizeof(S3::Particle)));
	gpuErrchk(cudaMemcpy(&particles, tmp, num * sizeof(S3::Particle), cudaMemcpyDeviceToHost));
}