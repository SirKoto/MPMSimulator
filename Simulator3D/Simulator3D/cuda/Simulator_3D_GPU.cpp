#include "Simulator_3D_GPU.h"


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

Simulator_3D_GPU::Simulator_3D_GPU(HYPERELASTICITY mode) : Simulator_3D(mode)
{

}

void Simulator_3D_GPU::step(float dt)
{
	if (!this->m_hasUpdated) {
		this->m_hasUpdated = true;
		updateGPUMemory();
	}
}

unsigned int Simulator_3D_GPU::dumpPositions(float* positions) const
{
	assert(false);
	return 0;
}

unsigned int Simulator_3D_GPU::dumpPositionsNormalized(float* positions)
{
	Simulator_3D_GPU::Particle* ptr = particles.data();
	storeParticles2Array(ptr, this->particles.size());

	return Simulator_3D::dumpPositionsNormalized(positions);
}

void Simulator_3D_GPU::updateGPUMemory()
{
	loadParticles2GPU(particles.data(), particles.size());
	size_t size = static_cast<size_t>(grid_size);
	loadGrid2GPU(size);
	loadPhysicsGrid2GPU(physicsGrid, size);
	loadProperties2GPU(v_properties.data(), v_properties.size());
}
