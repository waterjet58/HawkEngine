#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Hawk/Math/Quaternion.h>

namespace Hawk
{

	struct Transform2D {
		glm::vec2 position{0.f, 0.f};
		glm::vec2 scale{1.f, 1.f};;
		float rotation = 0.f;

		glm::mat2 mat2() { 
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);

			glm::mat2 rotMat{ {c, s}, { -s, c }};
			glm::mat2 scaleMat{{scale.x, 0.0f}, { 0.0f, scale.y }};

			return rotMat * scaleMat; 
		}
	};

	//TODO Quaternions for rotation, possible need to implement my own Quaternion class or use glm
	struct Transform3D {
		glm::vec3 position{};
		glm::vec3 scale{1.f,1.f,1.f};
		glm::vec3 rotation{}; //Degrees -> Radians

		glm::mat4 mat4() {
			auto transform = glm::translate(glm::mat4{1.f}, position); //offset the transform by the position

			transform = glm::scale(transform, scale); // scale the transform

			Quaternion quat(rotation);
			
			glm::quat myQuat;

			myQuat = glm::quat(quat.w, quat.x, quat.y, quat.z);

			transform = transform * glm::toMat4(myQuat);

			transform = glm::scale(transform, scale);

			return transform;

		}
	};



}