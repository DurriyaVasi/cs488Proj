#pragma once

#include "Background.hpp"

struct Scene {

	Scene()
			: node(NULL),
			  background(Background()){}

	SceneNode *node;
	Background background;
};
