#include "CascadedShadowCamera.h"
#include <algorithm>

namespace photon 
{
	CascadedShadowCamera::CascadedShadowCamera(RenderCamera* renderCamera /*= nullptr*/, LightBase* light /*= nullptr*/)
	{
		m_TrackedCamera = renderCamera;
		m_TrackedLight = light;
	}

	void CascadedShadowCamera::TrackLight(LightBase* light)
	{
		m_TrackedLight = light;
	}

	void CascadedShadowCamera::TrackCamera(RenderCamera* renderCamera)
	{
		m_TrackedCamera = renderCamera;
	}

	std::vector<std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX>> CascadedShadowCamera::GenerateShadowCameraMatrices(const std::vector<float>& splitRatio, Vector2 shadowMapSize)
	{
		if (!m_TrackedCamera || !m_TrackedLight)
			PHOTON_ASSERT(false, "No TrackCamera or TrackedLight!");

		m_SplitRatio = splitRatio;
		std::vector<std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX>> retMatrices(splitRatio.size());
		
		auto renderCameraFrustum = m_TrackedCamera->GetWorldFrustum();
		Vector3 corners[8];
		renderCameraFrustum.GetCorners((DirectX::XMFLOAT3*)corners);
		std::array<Vector3, 4> lastCorners = {corners[0], corners[1], corners[2], corners[3]};
		for(int i = 0; i < m_SplitRatio.size(); ++i)
		{
			float ratio = m_SplitRatio[i];
			std::array<Vector3, 4> farCorners = {
				Lerp(corners[0], corners[4], ratio),
				Lerp(corners[1], corners[5], ratio),
				Lerp(corners[2], corners[6], ratio),
				Lerp(corners[3], corners[7], ratio)
			};
			retMatrices[i] = GenerateSingleShadowCameraMatrix(lastCorners, farCorners, shadowMapSize);
			lastCorners = farCorners;
		}
		//std::array<Vector3, 4> farCorners = {
		//		corners[4],
		//		corners[5],
		//		corners[6],
		//		corners[7]
		//};
		//retMatrices.back() = GenerateSingleShadowCameraMatrix(lastCorners, farCorners);

		return retMatrices;
	}

	std::vector<std::array<Vector3, 8>> CascadedShadowCamera::GenerateCameraCSMCorners(
		const std::vector<float>& splitRatio)
	{
		if (!m_TrackedCamera || !m_TrackedLight)
			PHOTON_ASSERT(false, "No TrackCamera or TrackedLight!");

		m_SplitRatio = splitRatio;
		std::vector<std::array<Vector3, 8>> retFrustumCorners(splitRatio.size());

		auto renderCameraFrustum = m_TrackedCamera->GetWorldFrustum();
		Vector3 corners[8];
		renderCameraFrustum.GetCorners((DirectX::XMFLOAT3*)corners);
		std::array<Vector3, 4> lastCorners = { corners[0], corners[1], corners[2], corners[3] };
		for (int i = 0; i < m_SplitRatio.size(); ++i)
		{
			float ratio = m_SplitRatio[i];
			std::array<Vector3, 4> farCorners = {
				Lerp(corners[0], corners[4], ratio),
				Lerp(corners[1], corners[5], ratio),
				Lerp(corners[2], corners[6], ratio),
				Lerp(corners[3], corners[7], ratio)
			};

			std::ranges::copy(lastCorners, retFrustumCorners[i].begin());
			std::ranges::copy(farCorners, retFrustumCorners[i].begin() + 4);

			lastCorners = farCorners;
		}
		return retFrustumCorners;
	}

	std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX> 
		CascadedShadowCamera::GenerateSingleShadowCameraMatrix(const std::array<Vector3, 4>& nearCorners, 
			const std::array<Vector3, 4>& farCorners, Vector2 shadowMapSz)
	{
		using namespace DirectX;

		Vector3 frustumCenter = { 0.0f, 0.0f, 0.0f };
		for(int i = 0; i < 4; ++i)
		{
			frustumCenter += nearCorners[i] + farCorners[i];
		}
		frustumCenter /= 8.0f;

		Vector3 dir = m_TrackedLight->data.direction.normalisedCopy();
		Vector3 up = (abs(dir.dotProduct(Vector3::UNIT_Y)) > 0.99f) ? Vector3::UNIT_X : Vector3::UNIT_Y;
		XMMATRIX lightView = XMMatrixLookAtLH(
			XMLoadFloat3((XMFLOAT3*)&m_TrackedLight->data.position),
			XMLoadFloat3((XMFLOAT3*)&frustumCenter),
			XMLoadFloat3((XMFLOAT3*)&up));

		std::array<Vector3, 8> cornersInViewSpace;
		for(int i = 0; i < 4; ++i)
		{
			XMStoreFloat3((XMFLOAT3*)&cornersInViewSpace[i], XMVector3Transform(
				XMLoadFloat3((XMFLOAT3*)&nearCorners[i]), lightView));
			XMStoreFloat3((XMFLOAT3*)&cornersInViewSpace[i+4], XMVector3Transform(
				XMLoadFloat3((XMFLOAT3*)&farCorners[i]), lightView));
		}

		BoundingBox aabbInViewSpace;
		BoundingBox::CreateFromPoints(aabbInViewSpace, 8, (XMFLOAT3*)&cornersInViewSpace[0], sizeof(XMFLOAT3));
		
		Vector3 aabbCenter = aabbInViewSpace.Center;
		Vector3 aabbExtents = aabbInViewSpace.Extents;
		Vector3 minPos = aabbCenter - aabbExtents;
		Vector3 maxPos = aabbCenter + aabbExtents;

		float width = maxPos.x - minPos.x;
		float height = maxPos.y - minPos.y;
		float texelSizeX = width / shadowMapSz.x;
		float texelSizeY = height / shadowMapSz.y;

		// 把投影中心/最小点吸附到 texel 网格
		aabbCenter.x = floor(aabbCenter.x / texelSizeX) * texelSizeX;
		aabbCenter.y = floor(aabbCenter.y / texelSizeY) * texelSizeY;
		float farPlane = maxPos.z;
		float nearPlane = minPos.z;
		float epsilon = 0.0001f;

		if(nearPlane <= epsilon)
		{
			nearPlane = epsilon;
			if(farPlane < 0.5f + epsilon)
			{

				farPlane = 0.5f + epsilon;
			}
		}
		
		farPlane *= frustumScaleFactor;
		nearPlane /= frustumScaleFactor;

		XMMATRIX lightProj = XMMatrixOrthographicLH(width, height, nearPlane, farPlane);

		return std::make_tuple(lightView, lightProj);
	}

	LightBase* CascadedShadowCamera::GetLight()
	{
		return m_TrackedLight;
	}

	RenderCamera* CascadedShadowCamera::GetCamera()
	{
		return m_TrackedCamera;
	}

	photon::Vector3 CascadedShadowCamera::Lerp(const Vector3& pa, const Vector3& pb, float ratio)
	{
		return pa * (1 - ratio) + pb * ratio;
	}

}


