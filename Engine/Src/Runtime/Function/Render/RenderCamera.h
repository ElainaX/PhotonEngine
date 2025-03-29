#pragma once
#include "WindowSystem.h"
#include "Core/Math/Quaternion.h"

#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

namespace photon 
{
	class RenderCamera
	{
	public:
		RenderCamera(float _aspectRatio, float _znear = 0.1f, float _zfar = 100.0f)
			: aspectRatio(_aspectRatio), znear(_znear), zfar(_zfar){}


		Vector2 GetFov() const;

		void SetLookPoint(Vector3 point);
		void Rotate(Vector2 delta_xy);

		void MoveForward(float d);
		void MoveRight(float d);
		void MoveUp(float d);


		DirectX::XMMATRIX GetViewMatrix();
		DirectX::XMMATRIX GetProjMatrix();



	public:
		static const Vector3 X, Y, Z;

		// 四元数用作插值，暂时没啥用
		Quaternion rotationQ{ Quaternion::IDENTITY };
		Quaternion invRotationQ{ Quaternion::IDENTITY };



		float znear = 0.1f;
		float zfar = 100.0f;


		// 大部分时间只需要用yaw和pitch
		float yaw = 0.0f;
		float pitch = 0.0f;

		Vector3 pos = { 0.0f, 0.0f, -3.0f };
		 
		float aspectRatio = 1.0f;
		float fov = DirectX::XM_PIDIV4 * 3;
		static constexpr float s_MinFov{ 10.0f };
		static constexpr float s_MaxFov{ 89.0f };

		Vector3 right = { X };
		Vector3 up = { Y };
		Vector3 front = { Z };

	private:

		void UpdateCameraFromFront(Vector3 newFront);
		
		DirectX::BoundingFrustum m_CameraFrustum;

		static constexpr float s_MaxPitchAngle = DirectX::XMConvertToRadians(89.0f);
		static constexpr float s_MinPitchAngle = -DirectX::XMConvertToRadians(89.0f);
	};
}
