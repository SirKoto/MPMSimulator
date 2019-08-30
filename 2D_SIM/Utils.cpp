#include "Utils.h"

// create implementation
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>
#include <Windows.h>

namespace utils {

	inline void utilF::polarDecomposition2D(const glm::mat2& m, glm::mat2& s, glm::mat2& r)
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


	void utilF::writeImageToDisk(char const* filename, int id, int w, int h, int comp, const void* data)
	{
		// create dir
		if (!CreateDirectory("img_out", NULL) && !ERROR_ALREADY_EXISTS == GetLastError())
		{
			return; // abort!!
		}

		char concat[100];
		strcpy_s(concat, "img_out/");
		strcat_s(concat, filename);
		strcat_s(concat, std::to_string(id).c_str());
		strcat_s(concat, ".png");
		stbi_flip_vertically_on_write(1);
		stbi_write_png(concat, w, h, comp, data, 0);
	}
}