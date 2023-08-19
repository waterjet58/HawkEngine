#pragma once

#include "Transform.h"
#include "Hawk/Renderer/Model.h"
#include <glm/glm.hpp>

namespace Hawk {
	struct PointLight {
		Transform3D transform{};
		float intensity{1.f};
	};

}