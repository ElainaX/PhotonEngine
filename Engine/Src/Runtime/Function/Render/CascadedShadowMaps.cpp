#include "CascadedShadowMaps.h"

#include "Core/Math/MathFunction.h"
#include "RenderSystem.h"

namespace photon 
{


	CascadedShadowMaps::CascadedShadowMaps()
	{
		auto renderSystem = g_RuntimeGlobalContext.renderSystem;
		m_ResourceManager = renderSystem->GetResourceManager();
		m_Rhi = renderSystem->GetRHI().get();
	}

	void CascadedShadowMaps::CreateShadowMapResources(int width, int height, int _shadowMapNum /*= 1*/)
	{
		shadowMapNum = _shadowMapNum;
		shadowMapSize = Vector2i{ width, height };
		ReCreateShadowMapResources();
	}

	void CascadedShadowMaps::ReCreateShadowMapResources()
	{
		if(m_ShadowMapResources)
			m_ResourceManager->DestoryTexture2DArray(m_ShadowMapResources->guid);
		Texture2DArrayDesc desc;
		desc.format = shadowMapFormat;
		desc.depthOrArraySize = shadowMapNum;
		desc.width = shadowMapSize.x;
		desc.height = shadowMapSize.y;
		m_ShadowMapResources = m_ResourceManager->CreateTexture2DArray(desc);

		D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc;
		viewDesc.Texture2DArray.ArraySize = 1;
		viewDesc.Texture2DArray.MipSlice = 0;
		viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
		viewDesc.Flags = D3D12_DSV_FLAG_NONE;
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		for (int i = 0; i < shadowMapNum; ++i)
		{
			assert(m_ShadowMapResources);
			viewDesc.Texture2DArray.FirstArraySlice = i;
			m_ShadowMapDsvs[i] = m_Rhi->CreateDepthStencilView(m_ShadowMapResources.get(), &viewDesc);
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.ArraySize = shadowMapNum;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.MipLevels = 1;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		m_ShadowMapSrv = m_Rhi->CreateShaderResourceView(m_ShadowMapResources.get(), &srvDesc);	
	}

	std::vector<DepthStencilView*> CascadedShadowMaps::GetShadowMapsAsDepthStencilViews()
	{
		if(m_ShadowMapDsvs.empty())
		{
			ReCreateShadowMapResources();
		}
		return m_ShadowMapDsvs;
	}

	photon::ShaderResourceView* CascadedShadowMaps::GetShadowMapsAsShaderResourceView()
	{
		if(!m_ShadowMapSrv)
		{
			ReCreateShadowMapResources();
		}
		return m_ShadowMapSrv;
	}

}