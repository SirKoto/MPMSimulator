#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include <iostream>
#include <string>
namespace utils
{
	static unsigned int SCR_WIDTH = 800;
	static unsigned int SCR_HEIGHT = 600;

	static float DeltaTime = 0.0f;
	static float LastFrame = 0.0f;
	const static unsigned int maxParticles = 10000;


	inline glm::mat2 outerProduct(const glm::vec2& a, const glm::vec2& b)
	{
		glm::mat2 r(0.0f);

		r[0][0] = a[0] * b[0];
		r[0][1] = a[0] * b[1];
		r[1][0] = a[1] * b[0];
		r[1][1] = a[1] * b[1];
		return r;
	}

	inline void polarDecomposition2D(const glm::mat2& m, glm::mat2& s, glm::mat2& r)
	{
		float x = m[1][1] + m[0][0]; // trace
		float y = m[1][0] - m[0][1];
		float scale = 1.0f / std::sqrt(x * x + y * y);
		float c = x * scale;
		float d = y * scale;
		r[1][1] = c;
		r[0][0] = c;
		r[0][1] = -d;
		r[1][0] = d;
		s = glm::transpose(r) * m;

		if (s[0][0] != s[0][0])
		{
			std::cerr << "NAN in polar decom" << std::endl;
		}
	}

	// https://www.seas.upenn.edu/~cffjiang/research/svd/svd.pdf 
	// Find A = UEV^T where U and V are ortho and E diagonal
	inline void singularValueDecomposition(glm::mat2 m, glm::mat2& U, glm::mat2& E, glm::mat2& V)
	{
		

		glm::mat2 S;
		polarDecomposition2D(m, S, U); // S scale, U rotation
		float c, s;
		if (std::abs(S[0][1]) < 1e-6f)
		{
			E = S;
			c = 1;
			s = 0;
		}
		else
		{
			float tau = 0.5f * (S[0][0] - S[1][1]);
			float w = std::sqrt(tau * tau + S[0][1] * S[0][1]);
			float t = tau > 0 ? S[0][1] / (tau + w) : S[0][1] / (tau - w);
			c = 1.0f / std::sqrt(t * t + 1);
			s = -t * c;
			E[0][0] = c * c * S[0][0] - 2 * c * s * S[0][1] + s * s * S[1][1];
			E[1][1] = s * s * S[0][0] + 2 * c * s * S[0][1] + c * c * S[1][1];
		}

		if (E[0][0] < E[1][1])
		{
			std::swap(E[0][0], E[1][1]);
			V[0][0] = -s;
			V[0][1] = -c;
			V[1][0] = c;
			V[1][1] = -s;
		}
		else
		{
			V[0][0] = c;
			V[0][1] = -s;
			V[1][0] = s;
			V[1][1] = c;
		}

		V = glm::transpose(V);
		U = U * V;

		//set diagonal E
		E[0][1] = E[1][0] = 0;
	}

	class utilF
	{
	public:

		void static writeImageToDisk(char const* filename, int id, int w, int h, int comp, const void* data);
	};
}
#endif // UTILS_H