#pragma once 

#include <vector>
#include <memory>

#include "DX12RHI/DX12RHI.h"
#include "Core/Math/Vector2i.h"
#include "Core/Math/Vector4.h"
#include "Resource/Texture/Texture2DArray.h"
#include "Function/Global/RuntimeGlobalContext.h"

namespace photon 
{
	// shadow maps in GPU are stored as textureArray
	// So we need multiple renderTargets view and single ShaderResource view
	class CascadedShadowMaps
	{
	public:
		CascadedShadowMaps();

		void CreateShadowMapResources(int width, int height, int shadowMapNum = 1);
		void ReCreateShadowMapResources();
		std::vector<DepthStencilView*> GetShadowMapsAsDepthStencilViews();
		ShaderResourceView* GetShadowMapsAsShaderResourceView();

		Vector2i shadowMapSize;
		int shadowMapNum = 1;
		DXGI_FORMAT shadowMapFormat = DXGI_FORMAT_R24G8_TYPELESS;
	private:
		ResourceManager* m_ResourceManager = nullptr;
		RHI* m_Rhi = nullptr;

		std::shared_ptr<Texture2DArray> m_ShadowMapResources;
		std::vector<DepthStencilView*> m_ShadowMapDsvs;
		ShaderResourceView* m_ShadowMapSrv = nullptr;
	};


}