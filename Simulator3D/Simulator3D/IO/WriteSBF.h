#pragma once

#ifndef _WRITESBF_
#define _WRITESBF_

#include <string>
#include <iostream>
#include <fstream>

class WriteSBF
{
public:
	WriteSBF(const std::string& path, const unsigned long n_particles);
	
	~WriteSBF();

	void writeData3f(const float* data);

private:

	static constexpr unsigned long size_bulk = 100;

	const unsigned long n;
	const unsigned long rest;
	const unsigned long it;


	std::ofstream stream;

};

#endif // !_WRITESBF_
