#pragma once 

#include <vector>
#include <memory>

#include "DX12RHI/DX12RHI.h"
#include "Core/Math/Vector2i.h"
#include "Core/Math/Vector4.h"
#include "Resource/Texture/Texture2DArray.h"

namespace photon 
{
	// shadow maps in GPU are stored as textureArray
	// So we need multiple renderTargets view and single ShaderResource view
	class CascadedShadowMaps
	{
	public:
		CascadedShadowMaps();

		void CreateShadowMapResources(int width, int height, int _shadowMapNum = 1);
		void ReCreateShadowMapResources(int width, int height, int _shadowMapNum = 1);
		std::vector<DepthStencilView*> GetShadowMapsAsDepthStencilViews();
		ShaderResourceView* GetShadowMapsAsShaderResourceView();

		Vector2i shadowMapResolution;
		int shadowMapNum = 1;
		DXGI_FORMAT shadowMapFormat = DXGI_FORMAT_R24G8_TYPELESS; // 使用R24作为Depth的读取
	private:
		ResourceManager* m_ResourceManager = nullptr;
		RHI* m_Rhi = nullptr;

		std::shared_ptr<Texture2DArray> m_ShadowMapResources;
		std::vector<DepthStencilView*> m_ShadowMapDsvs;
		ShaderResourceView* m_ShadowMapSrv = nullptr;
	};


}