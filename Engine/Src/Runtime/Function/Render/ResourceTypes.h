#pragma once
#include <cstdint>

namespace photon
{
	struct Guid
	{
		uint64_t guid = 0;
		bool IsValid() const { return guid != 0; }
		friend bool operator==(const Guid& a, const Guid& b)
		{
			return a.guid == b.guid;
		}
		friend bool operator!=(const Guid& a, const Guid& b)
		{
			return !(a == b);
		}

		bool operator<(const Guid& other) const
		{
			return guid < other.guid;
		}
	};


	enum class ResourceType : uint8_t
	{
		Unknown = 0,
		Texture2D,
		Texture2DArray,
		Cubemap,
		Mesh,
		Material,
		Shader
	};

	// 这几个状态都是啥意思
	enum class CpuState : uint8_t
	{
		NotLoaded = 0,
		LoadedRaw,
		CPUReady,
		CPUCached
	};

	enum class GpuState : uint8_t
	{
		NoGPU = 0,
		UploadQueued,
		ResidentWarm,
		ResidentHot,
		EvictCandidate
	};

	enum class ResidencyPriority : uint8_t
	{
		Permanent = 0,
		High,
		Normal,
		Low
	};



}