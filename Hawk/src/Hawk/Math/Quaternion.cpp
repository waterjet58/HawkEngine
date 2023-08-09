#include "hwkPrecompiledHeader.h"
#include "Quaternion.h"

namespace Hawk {

	Quaternion::Quaternion()
	{

	}

	Quaternion::Quaternion(float pw, float px, float py, float pz)
	: w(pw), x(px), y(py), z(pz) {}

	Quaternion::Quaternion(glm::vec3 vector)
	{
		float px = vector.x * (glm::pi<float>() / 180.f);
		float py = vector.y * (glm::pi<float>() / 180.f);
		float pz = vector.z * (glm::pi<float>() / 180.f);

		float cr = cos(px * 0.5f); 
		float sr = sin(px * 0.5f); 
		float cp = cos(py * 0.5f); 
		float sp = sin(py * 0.5f); 
		float cy = cos(pz * 0.5f); 
		float sy = sin(pz * 0.5f); 

		w = cr * cp * cy + sr * sp * sy; 
		x = sr * cp * cy - cr * sp * sy;
		y = cr * sp * cy + sr * cp * sy;
		z = cr * cp * sy - sr * sp * cy;
	}
}