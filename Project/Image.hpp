#pragma once

#include "Background.hpp"
#include <glm/glm.hpp>
#include "Camera.hpp"

class Image {
	public:

        Background background;
        Camera camera;
	float perspectiveDegrees;

	Image()
		: background(Background()),
                  camera(Camera()),
                  perspectiveDegrees(0) {}

	Image(Background background, Camera camera, float perspectiveDegrees)
		: background(background),
		  camera(camera),
		  perspectiveDegrees(perspectiveDegrees) {}

	 
};

