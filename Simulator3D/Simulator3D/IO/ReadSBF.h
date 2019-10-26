#pragma once

#ifndef _READSBF_
#define _READSBF_

#include <string>
#include <iostream>
#include <fstream>

class ReadSBF
{
public:
	ReadSBF(const std::string& path);
	
	~ReadSBF();

	char ReadData(float* data);

	inline unsigned long GetNumberParticles() const
	{
		return n;
	}

private:

	static constexpr unsigned long size_bulk = 100;

	unsigned long n;
	unsigned long it;
	unsigned long rest;
	std::ifstream stream;

	void ReadData3f(float* data);

};

#endif // !_READSBF_
