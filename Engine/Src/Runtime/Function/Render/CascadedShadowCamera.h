#pragma once

#include "RenderCamera.h"
#include "Light.h"
#include "Core/Math/Vector3.h"

#include <vector>
#include <tuple>
#include <array>

namespace photon 
{
	/**
	 * 特殊Camera，Track一个灯光位置和一个Render Camera
	 */ 
	class CascadedShadowCamera
	{
	public:
		CascadedShadowCamera(RenderCamera* renderCamera = nullptr, LightBase* light = nullptr);

		void TrackLight(LightBase* light);
		void TrackCamera(RenderCamera* renderCamera);
		std::vector<std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX>> GenerateShadowCameraMatrices(const std::vector<float>& splitRatio, Vector2 shadowMapSize);
		std::vector<std::array<Vector3, 8>> GenerateCameraCSMCorners(const std::vector<float>& splitRatio);
		
		LightBase* GetLight();
		RenderCamera* GetCamera();

		Vector3 Lerp(const Vector3& pa, const Vector3& pb, float ratio);

		float frustumScaleFactor = 1.0f;

	private:
		std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX> GenerateSingleShadowCameraMatrix(const std::array<Vector3, 4>& nearCorners, const std::array<Vector3, 4>& farCorners, Vector2 shadowMapSz);


	private:
		LightBase* m_TrackedLight = nullptr;
		RenderCamera* m_TrackedCamera = nullptr;

		std::vector<float> m_SplitRatio;

		// 送去渲染的Camera属性
		std::vector<std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX>> m_ShadowCameraMatrices;
	};
}