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
	WriteSBF(const std::string& path, const unsigned long n_particles);
	
	~WriteSBF();

	void writeDataf(const float data, const char flag);

	void writeData3f(const float* data, const char flag = SBF_DATA);

	bool canWrite() const;

private:

	static constexpr unsigned long size_bulk = 100;

	const unsigned long n;
	const unsigned long rest;
	const unsigned long it;


	std::ofstream stream;

};

#endif // !_WRITESBF_
