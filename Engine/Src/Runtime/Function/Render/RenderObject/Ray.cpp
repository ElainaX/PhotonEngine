#include "Ray.h"

namespace photon 
{

	Ray::Ray(const Vector3& _origin, const Vector3& _dir)
	{
		origin = _origin;
		dir = _dir;
	}

	Ray::Ray()
	{
		origin = { .0f, .0f, .0f };
		dir = { .0f, .0f, 1.0f };
	}

	photon::Vector3 Ray::March(float t)
	{
		origin += dir * t;
		return origin;
	}

	photon::Vector3 Ray::At(float t)
	{
		return origin + dir * t;
	}

}