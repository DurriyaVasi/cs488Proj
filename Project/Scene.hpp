#pragma once

#include "Image.hpp"
#include <set>
#include <string>

struct Scene {
	SceneNode *node;
	std::set<std::string> textureFiles;
	std::set<std::string> textureNormalFiles;
	Image images[3];
	SceneNode *ballNode;
	SceneNode *paddleNode;
};
