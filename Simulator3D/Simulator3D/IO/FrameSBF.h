#pragma once

#ifndef _FRAMESBF_
#define _FRAMESBF_

template<typename T>
class FrameSBF {
public:
	FrameSBF() {
		data = nullptr;
	}

	~FrameSBF() = default;

	// Disable copy constructor
	FrameSBF(const FrameSBF& f) = delete;

	FrameSBF(const FrameSBF&& f) noexcept
	{
		this->data = f.data;
	}


	FrameSBF& operator=(const FrameSBF& f) = delete;

	void prepareData(const unsigned long number_elements) {
		if (data != nullptr)
		{
			delete[] data;
		}

		data = new T[number_elements];
	}

	void deleteData()
	{
		delete[] this->data;
	}

	T* ptr()
	{
		return data;
	}

	const T* const_ptr() const {
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
