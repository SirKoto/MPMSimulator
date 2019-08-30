#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace utils
{
	static unsigned int SCR_WIDTH = 800;
	static unsigned int SCR_HEIGHT = 600;



	const static unsigned int maxParticles = 5000;


	static float DeltaTime = 0.0f;
	static float LastFrame = 0.0f;


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
	}

	inline glm::mat2 outerProduct(const glm::vec2& a,const glm::vec2& b)
	{
		glm::mat2 r(0.0f);

		r[0][0] = a[0] * b[0];
		r[0][1] = a[0] * b[1];
		r[1][0] = a[1] * b[0];
		r[1][1] = a[1] * b[1];
		return r;
	}
}