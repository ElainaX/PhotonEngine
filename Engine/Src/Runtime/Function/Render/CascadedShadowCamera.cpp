#include "CascadedShadowCamera.h"

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

	std::vector<std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX>> CascadedShadowCamera::GenerateShadowCameraMatrices(const std::vector<float>& splitRatio)
	{
		if (!m_TrackedCamera || !m_TrackedLight)
			PHOTON_ASSERT(false, "No TrackCamera or TrackedLight!");

		m_SplitRatio = splitRatio;
		std::vector<std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX>> retMatrices(splitRatio.size()+1);
		
		auto renderCameraFrustum = m_TrackedCamera->GetFrustum();
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
			retMatrices[i] = GenerateSingleShadowCameraMatrix(lastCorners, farCorners);
			lastCorners = farCorners;
		}
		std::array<Vector3, 4> farCorners = {
				corners[4],
				corners[5],
				corners[6],
				corners[7]
		};
		retMatrices.back() = GenerateSingleShadowCameraMatrix(lastCorners, farCorners);

		return retMatrices;
	}

	std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX> 
		CascadedShadowCamera::GenerateSingleShadowCameraMatrix(const std::array<Vector3, 4>& nearCorners, 
			const std::array<Vector3, 4>& farCorners)
	{
		using namespace DirectX;

		Vector3 frustumCenter = { 0.0f, 0.0f, 0.0f };
		for(int i = 0; i < 4; ++i)
		{
			frustumCenter += nearCorners[i] + farCorners[i];
		}
		frustumCenter /= 8.0f;

		XMMATRIX lightView = XMMatrixLookAtLH(
			XMLoadFloat3((XMFLOAT3*)&m_TrackedLight->data.position),
			XMLoadFloat3((XMFLOAT3*)&frustumCenter),
			XMLoadFloat3((XMFLOAT3*)&Vector3::UNIT_Y));

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


