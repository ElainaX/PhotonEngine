#pragma once

namespace photon 
{
	enum class FrameResourceType
	{
		StaticModelFrameResource,
		DynamicModelFrameResource
	};


	class FrameResource
	{
	public:
		FrameResourceType GetType() const
		{
			return m_FrameResourceType;
		}

	
	protected:
		FrameResourceType m_FrameResourceType;
	};
}