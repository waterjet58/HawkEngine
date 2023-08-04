#pragma once

#include "Transform.h"
#include "Hawk/Renderer/Model.h"
#include <glm/glm.hpp>

namespace Hawk {

	struct Sprite {
		Transform2D transform{};
		glm::vec3 color{};
		std::shared_ptr<Model> model;
	};

	struct SimplePushConstantData {
		glm::mat2 transform{1.f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};
}