#pragma once

#include <string>

struct Texture {
	Texture()
			: hasTexture(false),
			  file(""){ }

	bool hasTexture;

	std::string & file;
};
