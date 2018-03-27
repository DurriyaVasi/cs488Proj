#pragma once

#include "Image.hpp"
#include "Animator.hpp"
#include <set>
#include <string>

struct Scene {
	Animator spaceship;
	std::set<std::string> textureFiles;
	std::set<std::string> textureNormalFiles;
	Image images[3];
	SceneNode *ballNode;
	SceneNode *paddleNode;
	SceneNode *startButton;
	SceneNode *playAgainButton;
	SceneNode *gameOverText;
	SceneNode *box;
	SceneNode *map;
	SceneNode *onePlayerButton;
	SceneNode *twoPlayerButton;
};
