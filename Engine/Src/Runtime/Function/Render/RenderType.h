#pragma once

#include <cstdint>

namespace photon 
{
	enum class RenderPipelineType : uint8_t
	{
		ForwardPipeline = 0,
		DifferedPipeline,
		RenderPipelineTypeCount

	};
}