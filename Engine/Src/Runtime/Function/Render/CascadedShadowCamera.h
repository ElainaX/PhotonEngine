#pragma once

#include "RenderCamera.h"
#include "Light.h"
#include "Core/Math/Vector3.h"

#include <array>
#include <tuple>

namespace photon 
{
	class CascadedShadowCamera
	{
		using QuadCorners = std::array<Vector3, 4>;
	public:
		CascadedShadowCamera(int cascadedNum, LightBase* lightTracked = nullptr);

		void TrackLight(LightBase* light);
		void SetCascadedNum(int num);
		std::tuple<QuadCorners, QuadCorners> GetLightCameraFrustumCorners();

	private:

	private:
		std::vector<QuadCorners> m_FrustumCorners;
		LightBase* m_TrackedLight = nullptr;
	};
}