#pragma once
#include "Macro.h"
#include <DirectXCollision.h>

namespace photon 
{

	struct Bounds
	{
		virtual Bounds Intersect(const Bounds& rhs) = 0;
		virtual Bounds Union(const Bounds& rhs) = 0;
		virtual bool isIntersect(const Bounds& rhs) = 0;
	};


}
