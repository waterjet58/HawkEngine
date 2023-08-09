#pragma once

#include "Transform.h"
#include "Hawk/Renderer/Model.h"
#include <glm/glm.hpp>

namespace Hawk {
	struct Mesh {
		Transform3D transform{};
		glm::vec3 color{};
		std::shared_ptr<Model> model;
	};

	struct MeshSimplePushConstantData {
		glm::mat4 transform{1.f};
		alignas(16) glm::vec3 color;
	};
}