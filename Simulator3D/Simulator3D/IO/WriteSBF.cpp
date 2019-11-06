#include "WriteSBF.h"
#include "../Utils.h"

WriteSBF::WriteSBF(const std::string& path, const unsigned long n_particles) :
	stream(path.c_str(), std::ios::out | std::ios::trunc | std::ios_base::binary),
	n(n_particles), rest(n_particles % size_bulk), it(n_particles / size_bulk)
{

	if (!canWrite()) // if the stream does not work... disable
		return;

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
	if(canWrite())
	{
		char flag = SBF_EOF;
		stream.write(&flag, 1);
		stream.flush();
	}
	stream.close();
}

void WriteSBF::writeDataf(const float data, const char flag)
{
	if (!canWrite())
		return;
	stream.write(&flag, 1);

	char buff[sizeof(float)];
	std::memcpy(buff, &data, sizeof(float));

	stream.write(buff, sizeof(float));
}

void WriteSBF::writeData3f(const float* data, const char flag)
{
	if (!canWrite())
		return;
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

bool WriteSBF::canWrite() const
{
	if (stream)
		return true;
	else 
		return false;
}
