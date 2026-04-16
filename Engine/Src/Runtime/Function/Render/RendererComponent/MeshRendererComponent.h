#pragma once 
#include <vector>

#include "Function/Render/ResourceHandle.h"

namespace photon
{
	struct MeshRendererComponent
	{
		MeshHandle mesh;

		// 覆盖材质表：slot -> material
		std::vector<MaterialHandle> overrideMaterials;

		bool castShadow = true;
		bool visible = true;
	};
}
 