#pragma once

#include "Function/Global/RuntimeGlobalContext.h"
#include "CascadedShadowMaps.h"
#include "CascadedShadowCamera.h"
#include "Define.h"


namespace photon 
{

	class CascadedShadowManager
	{
	public:
		CascadedShadowManager();
		void Initialize(Vector2i resolution, int cascadedNum = 1);
		void Track(LightBase* light, RenderCamera* camera);
		void Resize(Vector2i resolution);

		Vector2i GetResolution();

		std::vector<std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX>> GetViewAndProjMatrices(const std::vector<float>& spliters);
		std::vector<std::array<Vector3, 8>> GenerateCameraCSMCorners(const std::vector<float>& splitRatio);
		std::vector<DepthStencilView*> GetAllDepthStencilViews();
		DepthStencilView* GetDepthStencilView(int index);
		ShaderResourceView* GetShaderResourceView();

	private:
		int m_CascadedNum = 0;
		CascadedShadowMaps m_ShadowMapResource;
		CascadedShadowCamera m_ShadowMapCamera;
		std::vector<std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX>> m_LastRetMatrices;
	};


}