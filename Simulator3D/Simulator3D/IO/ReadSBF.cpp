#include "ReadSBF.h"
#include "../Utils.h"

ReadSBF::ReadSBF(const std::string& path) :
	stream(path.c_str(), std::ios::in | std::ios_base::binary)
{
	if (!canRead())
		return;

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

char ReadSBF::ReadNextFlag()
{
	return ReadNextFlag(true);
}

bool ReadSBF::canRead() const
{
	if (stream)
		return true;
	else
		return false;
}

char ReadSBF::ReadNextFlag(bool KeepPosition)
{
	if (!canRead())
		return SBF_ERROR;

	// read first flag
	char flag;
	stream.read(&flag, 1);

	if (KeepPosition)
	{
		stream.unget();
	}
	return flag;
}

void ReadSBF::ReadData3f(float* data)
{
	if (!canRead())
		return;

	char bloat[sizeof(float) * 3 * size_bulk];
	//copy data
	unsigned long i;
	for (i = 0; i < it; ++i)
	{
		stream.read(bloat, sizeof(float) * 3 * size_bulk);

		std::memcpy((data + (3ULL * i * size_bulk)), bloat, sizeof(float) * 3 * size_bulk);
	}

	// copy the rest
	stream.read(bloat, sizeof(float) * 3 * rest);
	std::memcpy((data + (3ULL * i * size_bulk)), bloat, sizeof(float) * 3 * rest);
}

float ReadSBF::ReadDataf()
{
	if (!canRead())
		return 0.0f;

	char buff[sizeof(float)];
	stream.read(buff, sizeof(float));
	float ret;
	std::memcpy(&ret, buff, sizeof(float));
	return ret;
}

void ReadSBF::ReadData3f(FrameSBF<float>& frame)
{
	//create and reserve memory for frame
	frame.prepareData(3 * n * sizeof(float));
	ReadData3f(frame.ptr());
}