#include "Utils.h"

#include <gif-h/gif.h>

// create implementation
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>


#include <boost/filesystem.hpp>

#include <cstring>

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
		using namespace std;
		strcpy(concat, "img_out/");
		strcat(concat, filename);
		strcat(concat, std::to_string(id).c_str());
		strcat(concat, ".png");
		stbi_flip_vertically_on_write(1);
		stbi_write_png(concat, w, h, comp, data, 0);
	}

	void utilF::writeFramesAsGif(const std::vector<FrameSBF<float>>& frames, SimVisualizer& viewer, int milisPerFrame, std::string fileName)
	{
		// add "gif" to filename if it does not have it
		if (fileName.size() < 6 || fileName.compare(fileName.size() - 4, 4, ".sbf") != 0)
		{
			fileName.append(".gif");
		}

		// create dir
		if (!createDir("gifs"))
			return;

		GifWriter g;
		GifBegin(&g, ("gifs/" + fileName).c_str(), viewer.GetWidth(), viewer.GetHeight(), 1);

		uint8_t* data = new uint8_t[4 * static_cast<size_t>(viewer.GetWidth() * viewer.GetHeight())];
		uint8_t* data_cpy = new uint8_t[4 * static_cast<size_t>(viewer.GetWidth() * viewer.GetHeight())];

		for (int i = 0; i < frames.size(); i += 1)
		{
			viewer.updateParticlePositions(frames[i]);
			viewer.draw();
			glReadPixels(0, 0, viewer.GetWidth(), viewer.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, data);

			// flip image
			for(int y = 0; y < viewer.GetHeight(); ++y)
			{
				std::memcpy(data_cpy + 4 * (viewer.GetHeight() - y -1) * viewer.GetWidth(),
					data + 4 * y * viewer.GetWidth(),
					4 * viewer.GetWidth());
			}
			GifWriteFrame(&g, data_cpy, viewer.GetWidth(), viewer.GetHeight(), 1);
		}

		GifEnd(&g);
		delete[] data, data_cpy;
	}


}
