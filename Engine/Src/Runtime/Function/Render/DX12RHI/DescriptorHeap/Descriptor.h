#pragma once
#include "Function/Render/DX12RHI/DX12Define.h"
#include "Resource/DXResourceHeader.h"
#include <cstdint>


namespace photon 
{
	

	struct DescriptorHandle
	{
		uint32_t page = 0;
		uint32_t slot = 0;
		uint32_t generation = 0;
		DescriptorHeapKind heapKind = DescriptorHeapKind::Unknown;

		bool IsValid() const
		{
			return generation != 0 && heapKind != DescriptorHeapKind::Unknown;
		}

		auto operator<=>(const DescriptorHandle&) const = default;
	};

	struct FrameDescriptorHandle
	{
		uint32_t index = UINT32_MAX;
		DescriptorHeapKind heapKind = DescriptorHeapKind::Unknown;

		bool IsValid() const
		{
			return index != UINT32_MAX;
		}

		auto operator<=>(const FrameDescriptorHandle&) const = default;
	};

}
