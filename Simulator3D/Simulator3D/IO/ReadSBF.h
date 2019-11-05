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
	ReadSBF(const std::string& path);
	
	~ReadSBF();


	char ReadNextFlag(bool KeepPosition = true);


	float ReadDataf();
	
	void ReadData3f(FrameSBF<float>& frame);
	
	void ReadData3f(float* data);

	inline unsigned long GetNumberParticles() const
	{
		return n;
	}

	bool canRead() const;

private:

	static constexpr unsigned long size_bulk = 100;

	unsigned long n;
	unsigned long it;
	unsigned long rest;
	std::ifstream stream;



};

#endif // !_READSBF_
