#include "RenderItem.h"

namespace photon 
{
	FrameResourceType CommonRenderItem::s_FrameResourceType = FrameResourceType::StaticModelFrameResource;

	std::string CommonRenderItem::GetGameObjectType()
	{
		return "CommonRenderItem";
	}

}