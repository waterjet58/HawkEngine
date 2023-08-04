#pragma once
#include <glm/glm.hpp>

namespace Hawk
{

	struct Transform2D {
		glm::vec2 position;
		glm::vec2 scale;
		float rotation;

		glm::mat2 mat2() { 
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);

			glm::mat2 rotMat{ {c, s}, { -s, c }};
			glm::mat2 scaleMat{{scale.x, 0.0f}, { 0.0f, scale.y }};

			return rotMat * scaleMat; 
		}
	};

	//TODO
	struct Transform3D {
		glm::vec3 position;
	};

}