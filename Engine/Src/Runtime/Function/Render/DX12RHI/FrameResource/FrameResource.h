#pragma once

namespace photon 
{
	class RHI;
	enum class FrameResourceType
	{
		StaticModelFrameResource,
		DynamicModelFrameResource
	};


	struct FrameResourceDesc
	{
		virtual ~FrameResourceDesc(){}
	};

	class FrameResource
	{
	public:
		FrameResourceType GetType() const
		{
			return m_FrameResourceType;
		}
		virtual ~FrameResource() {}
	
	protected:
		FrameResourceType m_FrameResourceType;
		RHI* m_Rhi = nullptr;
	};
}