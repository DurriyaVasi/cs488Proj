#pragma once

#include <string>

struct Background {
	Background()
			: faces({"", "", "", "", "", ""}) { }

	std::string faces[6];
};
