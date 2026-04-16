#include "CascadedShadowManager.h"

namespace photon 
{


	CascadedShadowManager::CascadedShadowManager()
	{

	}

	void CascadedShadowManager::Initialize(Vector2i resolution, int cascadedNum /*= 1*/)
	{
		m_CascadedNum = cascadedNum > MaxCascadedNum ? MaxCascadedNum : cascadedNum;
		m_ShadowMapResource.CreateShadowMapResources(resolution.x, resolution.y, m_CascadedNum);
	}

	void CascadedShadowManager::Track(LightBase* light, RenderCamera* camera)
	{
		m_ShadowMapCamera.TrackCamera(camera);
		m_ShadowMapCamera.TrackLight(light);
	}

	void CascadedShadowManager::Resize(Vector2i resolution)
	{
		m_ShadowMapResource.ReCreateShadowMapResources(resolution.x, resolution.y, m_CascadedNum);
	}

	photon::Vector2i CascadedShadowManager::GetResolution()
	{
		return m_ShadowMapResource.shadowMapResolution;
	}

	std::vector<std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX>> CascadedShadowManager::GetViewAndProjMatrices(const std::vector<float>& spliters)
	{
		if (spliters.empty() && !m_LastRetMatrices.empty())
			return m_LastRetMatrices;

		m_LastRetMatrices = m_ShadowMapCamera.GenerateShadowCameraMatrices(spliters, m_ShadowMapResource.shadowMapResolution);
		return m_LastRetMatrices;
	}

	std::vector<std::array<Vector3, 8>> CascadedShadowManager::GenerateCameraCSMCorners(const std::vector<float>& splitRatio)
	{
		assert(!splitRatio.empty());
		return m_ShadowMapCamera.GenerateCameraCSMCorners(splitRatio);
	}

	std::vector<photon::DepthStencilView*> CascadedShadowManager::GetAllDepthStencilViews()
	{
		return m_ShadowMapResource.GetShadowMapsAsDepthStencilViews();
	}

	photon::DepthStencilView* CascadedShadowManager::GetDepthStencilView(int index)
	{
		return m_ShadowMapResource.GetShadowMapsAsDepthStencilViews()[index];
	}

	photon::ShaderResourceView* CascadedShadowManager::GetShaderResourceView()
	{
		return m_ShadowMapResource.GetShadowMapsAsShaderResourceView();
	}

}