#pragma once

#include <glm/glm.hpp>

namespace Hawk
{

	struct Quaternion {

		float w = 0.f;
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;

		Quaternion();
		
		Quaternion(float w, float x, float y, float z);

		Quaternion(const glm::vec3 vector);

		Quaternion& operator*=(const Quaternion& quat2);

		Quaternion operator*(const Quaternion& quat2);

        bool operator==(const Quaternion & quat2) const;

		bool operator!=(const Quaternion & quat2) const;

		Quaternion& operator*=(float val);

		Quaternion& operator/=(float val);

		Quaternion eulerToQuaternion(float x, float y, float z);

		bool isNear(const Quaternion& quat2, const float& epsilon = 0.0001f) const;

		float norm() const;

		Quaternion normalize() const;

		Quaternion inverse() const;


	};

}