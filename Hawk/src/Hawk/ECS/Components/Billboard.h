#pragma once

#include "Transform.h"
#include "Hawk/Renderer/Model.h"
#include <glm/glm.hpp>

namespace Hawk {
	struct Billboard {
		Transform3D transform{ {0.f,0.f,0.f}, {1.f,1.f,1.f}, {0.f,0.f,0.f} }; // Pos, Scale, Rot
		glm::vec3 color{};
		//Sprite?
	};

}