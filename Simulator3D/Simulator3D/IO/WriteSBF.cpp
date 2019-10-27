#include "WriteSBF.h"
#include "../Utils.h"

WriteSBF::WriteSBF(const std::string& path, const unsigned long n_particles) :
	stream(path.c_str(), std::ios::out | std::ios::trunc | std::ios_base::binary),
	n(n_particles), rest(n_particles % size_bulk), it(n_particles / size_bulk)
{
	// write number of particles
	{
		char data[sizeof(n_particles)];
		std::memcpy(data, &n_particles, sizeof(n_particles));
		stream.write(data, sizeof(n_particles));
	}
	stream.flush();


}

WriteSBF::~WriteSBF()
{
	// write EOF 
	{
		char flag = SBF_EOF;
		stream.write(&flag, 1);
	}
	stream.flush();
	stream.close();
}

void WriteSBF::writeData3f(const float* data, const char flag)
{
	char bloat[sizeof(float) * 3 * size_bulk];

	// write some flag at the start
	{
		stream.write(&flag, 1);
	}

	// copy data
	unsigned long i;
	for (i = 0; i < it; ++i)
	{
		// use pointer arithmetic to add to the float array
		std::memcpy(bloat, (data + (3ULL * i * size_bulk)), sizeof(float) * 3 * size_bulk);
		stream.write(bloat, sizeof(float) * 3 * size_bulk);
	}

	// copy the rest
	std::memcpy(bloat, (data + (3ULL * i * size_bulk)), sizeof(float) * 3 * rest);
	stream.write(bloat, sizeof(float) * 3 * rest);

	stream.flush();
}
