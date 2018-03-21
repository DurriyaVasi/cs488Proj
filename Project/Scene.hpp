#pragma once

#include "Background.hpp"
#include <set>
#include <string>

struct Scene {

	Scene()
			: node(NULL),
			  background(Background()){}

	SceneNode *node;
	Background background;
	std::set<std::string> textureFiles;
	std::set<std::string> textureNormalFiles;
};
