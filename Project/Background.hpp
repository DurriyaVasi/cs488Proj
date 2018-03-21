#pragma once

#include <string>

struct Background {
	Background()
			: faces({"", "", "", "", "", ""}),
			  hasSkybox(false) { }

	std::string faces[6];
	bool hasSkybox;
};
