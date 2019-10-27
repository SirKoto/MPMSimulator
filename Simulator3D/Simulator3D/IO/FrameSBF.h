#pragma once

#ifndef _FRAMESBF_
#define _FRAMESBF_

template<typename T>
class FrameSBF {
public:
	FrameSBF(const unsigned long number_elements) {
		data = new T[number_elements];
	}

	~FrameSBF()
	{
		delete[] data;
	}

	// Disable copy constructor
	FrameSBF(const FrameSBF& f) = delete;
	FrameSBF& operator=(const FrameSBF& f) = delete;


	T* ptr()
	{
		return data;
	}

	T& operator[](const std::size_t pos)
	{
		return data[pos];
	}

private:
	T* data;
};

#endif // !_FRAMESBF_
