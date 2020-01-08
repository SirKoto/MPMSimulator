#pragma once

#ifndef _READSBF_
#define _READSBF_

#include <string>
#include <iostream>
#include <fstream>

#include "FrameSBF.h"

class ReadSBF
{
public:
	// path = file to read
	ReadSBF(const std::string& path);
	
	~ReadSBF();

	// know which is the next data to be read
	// keepPosition = false if the flag has to be deleted
	char ReadNextFlag(bool KeepPosition = true);

	// Read next 4 bytes
	float ReadDataf();
	
	// read next n_particles * 4 bytes and put the information onto a frame
	void ReadData3f(FrameSBF<float>& frame);
	
	// reald next n_particles * 4 bytes
	void ReadData3f(float* data);

	// Get the number of particles in the system to be read
	inline unsigned long GetNumberParticles() const
	{
		return n;
	}

	// Is the reader able to read data from file?
	bool canRead() const;

private:

	static constexpr unsigned long size_bulk = 100;

	unsigned long n;
	unsigned long it;
	unsigned long rest;

	// input stream
	std::ifstream stream;
};

#endif // !_READSBF_
