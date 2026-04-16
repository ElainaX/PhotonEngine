#pragma once
#include "Function/Render/RenderObject/ImporterSettings.h"

namespace photon
{
	struct TextureCpuData
	{
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t depth = 1;
		uint32_t arraySize = 1;
		uint32_t mipCount = 1;
		TextureDimension dimension = TextureDimension::Tex2D;
		TextureFormat format = TextureFormat::Unknown;

		std::vector<std::byte> pixels;

		bool Empty() const
		{
			return pixels.empty();
		}

		void Clear()
		{
			pixels.clear();
			width = height = depth = arraySize = mipCount = 0;
			format = TextureFormat::Unknown;
		}
	};
}
