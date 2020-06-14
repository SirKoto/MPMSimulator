#pragma once
#include "../Simulator_3D.h"

class Simulator_3D_GPU : public Simulator_3D
{
public:

	Simulator_3D_GPU(HYPERELASTICITY mode);

	void step(float dt) override;

	// never call this!!!!!
	unsigned int dumpPositions(float* positions) const override;

	unsigned int dumpPositionsNormalized(float* positions) override;

private:

	void updateGPUMemory();

	bool m_hasUpdated = false;
};
typedef Simulator_3D_GPU S3;

