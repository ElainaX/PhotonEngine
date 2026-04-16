#include "CascadedShadowMaps.h"

#include "Core/Math/MathFunction.h"
#include "RenderSystem.h"

namespace photon 
{


	CascadedShadowMaps::CascadedShadowMaps()
	{
		/*auto renderSystem = g_RuntimeGlobalContext.renderSystem;
		m_ResourceManager = renderSystem->GetResourceManager();
		m_Rhi = renderSystem->GetRHI().get();*/
	}

	void CascadedShadowMaps::CreateShadowMapResources(int width, int height, int _shadowMapNum /*= 1*/)
	{
		//shadowMapNum = _shadowMapNum;
		//shadowMapResolution = Vector2i{ width, height };
		//m_ShadowMapDsvs = std::vector<DepthStencilView*>(shadowMapNum, nullptr);

		//DXTexture2DArrayDesc desc;
		//desc.flag = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		//desc.format = shadowMapFormat;
		//desc.clearValueFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//desc.arraySize = shadowMapNum;
		//desc.width = shadowMapResolution.x;
		//desc.height = shadowMapResolution.y;
		//m_ShadowMapResources = m_ResourceManager->CreateTexture2DArray(desc);

		//D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc;
		//viewDesc.Texture2DArray.ArraySize = 1;
		//viewDesc.Texture2DArray.MipSlice = 0;
		//viewDesc.Texture2DArray.FirstArraySlice = 0;
		//viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
		//viewDesc.Flags = D3D12_DSV_FLAG_NONE;
		//viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//for (int i = 0; i < shadowMapNum; ++i)
		//{
		//	assert(m_ShadowMapResources);
		//	viewDesc.Texture2DArray.FirstArraySlice = i;
		//	m_ShadowMapDsvs[i] = m_Rhi->CreateDepthStencilView(m_ShadowMapResources.get(), &viewDesc, m_ShadowMapDsvs[i]);
		//}

		//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		//srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		//srvDesc.Texture2DArray.ArraySize = shadowMapNum;
		//srvDesc.Texture2DArray.FirstArraySlice = 0;
		//srvDesc.Texture2DArray.MipLevels = 1;
		//srvDesc.Texture2DArray.MostDetailedMip = 0;
		//srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		//srvDesc.Texture2DArray.PlaneSlice = 0; // TextureArray必須指定PlaneSlice
		//m_ShadowMapSrv = m_Rhi->CreateShaderResourceView(m_ShadowMapResources.get(), &srvDesc);
	}

	void CascadedShadowMaps::ReCreateShadowMapResources(int width, int height, int _shadowMapNum /*= 1*/)
	{
		/*if(width != shadowMapResolution.x || height != shadowMapResolution.y || shadowMapNum != _shadowMapNum)
		m_ResourceManager->DestoryTexture2DArray(m_ShadowMapResources.get());
		CreateShadowMapResources(width, height, shadowMapNum);*/
	}

	std::vector<DepthStencilView*> CascadedShadowMaps::GetShadowMapsAsDepthStencilViews()
	{
		/*if(m_ShadowMapDsvs.empty())
		{
			PHOTON_ASSERT(false, "Error When Get DepthStencilView, shadowMap haven't created!");
		}
		return m_ShadowMapDsvs;*/
		return {};
	}

	photon::ShaderResourceView* CascadedShadowMaps::GetShadowMapsAsShaderResourceView()
	{
		/*if(!m_ShadowMapSrv)
		{
			PHOTON_ASSERT(false, "Error When Get ShaderResourceView, shadowMap haven't created!")
		}
		return m_ShadowMapSrv;*/
		return nullptr;
	}

}