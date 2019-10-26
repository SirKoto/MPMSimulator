#include "ReadSBF.h"
#include "../Utils.h"

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

char ReadSBF::ReadData(float* data)
{

	// read first flag
	char flag;
	stream.read(&flag, 1);


	switch (flag)
	{
	case SBF_DATA:
		ReadData3f(data);
		break;

	case SBF_EOF:

	default:
		break;
	}

	return flag;
}

void ReadSBF::ReadData3f(float* data)
{
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
}