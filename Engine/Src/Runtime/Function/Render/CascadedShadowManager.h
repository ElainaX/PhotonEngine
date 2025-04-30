#pragma once

#include "Function/Global/RuntimeGlobalContext.h"
#include "CascadedShadowMaps.h"
#include "CascadedShadowCamera.h"

namespace photon 
{
	class CascadedShadowManager
	{
	public:
		CascadedShadowManager();

	private:
		CascadedShadowMaps m_ShadowMapResource;
		std::vector<CascadedShadowCamera> m_ShadowMapCameras;
	};


}