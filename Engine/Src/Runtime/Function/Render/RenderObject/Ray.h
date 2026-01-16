#pragma once
#include "Core/Math/Vector4.h"

namespace photon 
{
	struct Ray
	{
		Ray(const Vector3& _origin, const Vector3& _dir);
		Ray();

		Vector3 March(float t); // 让本Ray传播t时间
		Vector3 At(float t); // t时间传播后的位置


		Vector3 origin;
		Vector3 dir;
	};
}

