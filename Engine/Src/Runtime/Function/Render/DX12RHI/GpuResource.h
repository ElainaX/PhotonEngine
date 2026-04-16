#pragma once
#include <cstdint>
#include <compare>


namespace photon
{
	struct GpuResourceHandle
	{
		std::uint64_t index = 0;
		uint32_t generation = 0;
		// generation = 0 表示非有效handle
		bool IsValid() const { return generation != 0; }
		auto operator<=>(const GpuResourceHandle&) const = default;
	};

	struct GpuResourceSlot
	{
		uint32_t generation = 1;
		bool occupied = false;
	};
}
