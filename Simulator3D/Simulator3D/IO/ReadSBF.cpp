#include "ReadSBF.h"

ReadSBF::ReadSBF(const std::string& path) :
	stream(path.c_str(), std::ios::in | std::ios_base::binary)
{
	// Read number of particles
	{
		char data[sizeof(n)];
		stream.read(data, sizeof(n));
		std::memcpy(&n, data, sizeof(n));
	}

	it = n / size_bulk;
	rest = n % size_bulk;
}

ReadSBF::~ReadSBF()
{
	stream.close();
}

bool ReadSBF::ReadData3f(float* data)
{

	// read first flag
	{
		char flag;
		stream.read(&flag, 1);
		if (!flag)
			return false;
	}
	char bloat[sizeof(float) * 3 * size_bulk];
	//copy data
	unsigned long i;
	for (i = 0; i < it; ++i)
	{
		stream.read(bloat, sizeof(float) * 3 * size_bulk);

		std::memcpy((data + (3 * i * size_bulk)), bloat, sizeof(float) * 3 * size_bulk);
	}

	// copy the rest
	stream.read(bloat, sizeof(float) * 3 * rest);
	std::memcpy((data + (3 * i * size_bulk)), bloat, sizeof(float) * 3 * rest);

	return true;
}
