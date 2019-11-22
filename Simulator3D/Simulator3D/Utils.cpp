#include "Utils.h"

// create implementation
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>


#include <boost/filesystem.hpp>


namespace utils {

	
	bool utilF::createDir(const std::string& dirName)
	{
		using namespace boost::filesystem;
		path path(dirName);
		bool ret = true;
		if (!exists(path))
		{
			ret = boost::filesystem::create_directory(path);
		}

		return ret;
	}


	void utilF::writeImageToDisk(char const* filename, int id, int w, int h, int comp, const void* data)
	{
		// create dir
		if (!createDir("img_out"))
		{
			std::cout << "ERROR::CANNOT CREATE DIR" << std::endl;
			return;
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