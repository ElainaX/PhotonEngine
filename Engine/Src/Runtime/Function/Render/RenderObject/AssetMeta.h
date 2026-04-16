#pragma once
#include <filesystem>

#include "Function/Render/ResourceTypes.h"


namespace photon
{
	struct AssetMeta
	{
		Guid guid;
		std::filesystem::path sourcePath;
		std::string debugName;
	};
}
