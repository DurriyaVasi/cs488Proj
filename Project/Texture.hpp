#pragma once

#include <string>

struct Texture {
	Texture()
			: hasTexture(false),
			  file(""),
			  hasBumps(false),
			  normalFile(""){ }

	bool hasTexture;

	std::string file;

	bool hasBumps;
	
	std::string normalFile;
};
