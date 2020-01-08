#pragma once

#ifndef _WRITESBF_
#define _WRITESBF_

#include <string>
#include <iostream>
#include <fstream>

#include "../Utils.h"

class WriteSBF
{
public:
	// path = file to open
	// n_particles = number of particles in the system at any time
	WriteSBF(const std::string& path, const unsigned long n_particles);
	
	~WriteSBF();

	// Write a Float of data (4 bytes)
	void writeDataf(const float data, const char flag);

	// Write n_particles * 4 bytes of data
	void writeData3f(const float* data, const char flag = SBF_DATA);

	// Is the writer able to write?
	bool canWrite() const;

private:

	// Size of bytes transsferred at the same time
	static constexpr unsigned long size_bulk = 100;

	const unsigned long n;
	const unsigned long rest;
	const unsigned long it;

	// Output stream
	std::ofstream stream;

};

#endif // !_WRITESBF_
