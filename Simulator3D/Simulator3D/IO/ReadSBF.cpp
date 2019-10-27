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
char ReadSBF::ReadData(FrameSBF<float>& frame)
{

	// read first flag
	char flag;
	stream.read(&flag, 1);


	if (flag == SBF_DATA)
	{
		ReadData3f(frame);
	}
	else if (flag == SBF_COLOR)
	{
		ReadData3f(frame);
	}
	else if (flag == SBF_EOF)
	{
		//NOTHING
	}

	return flag;
}

void ReadSBF::ReadData3f(FrameSBF<float>& frame)
{
	//create and reserve memory for frame
	frame.prepareData(3 * n * sizeof(float));

	char bloat[sizeof(float) * 3 * size_bulk];
	//copy data
	unsigned long i;
	for (i = 0; i < it; ++i)
	{
		stream.read(bloat, sizeof(float) * 3 * size_bulk);

		std::memcpy((frame.ptr() + (3ULL * i * size_bulk)), bloat, sizeof(float) * 3 * size_bulk);
	}

	// copy the rest
	stream.read(bloat, sizeof(float) * 3 * rest);
	std::memcpy((frame.ptr() + (3ULL * i * size_bulk)), bloat, sizeof(float) * 3 * rest);
}