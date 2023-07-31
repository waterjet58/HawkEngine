#pragma once
#include <glm/glm.hpp>

namespace Hawk
{

	struct Transform2D {
		glm::vec2 position;
		glm::mat2 mat2 {1.f};
	};

	//TODO
	struct Transform3D {
		glm::vec3 position;
	};

}