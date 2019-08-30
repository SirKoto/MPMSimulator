#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

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

	class utilF
	{
	public:


		inline static void polarDecomposition2D(const glm::mat2& m, glm::mat2& s, glm::mat2& r);

		void static writeImageToDisk(char const* filename, int id, int w, int h, int comp, const void* data);
	};
}
#endif // UTILS_H