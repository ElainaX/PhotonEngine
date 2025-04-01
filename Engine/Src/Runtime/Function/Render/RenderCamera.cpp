#include "RenderCamera.h"
#include "RenderSystem.h"
#include <cmath>

using namespace DirectX;

namespace photon 
{
	const Vector3 RenderCamera::X = { 1.0f, 0.0f, 0.0f };
	const Vector3 RenderCamera::Y = { 0.0f, 1.0f, 0.0f };
	const Vector3 RenderCamera::Z = { 0.0f, 0.0f, 1.0f };


	Vector2 RenderCamera::GetFov() const
	{
		return Vector2(fov, fov * aspectRatio);
	}

	void RenderCamera::SetLookPoint(Vector3 point)
	{
		UpdateCameraFromFront(point - pos);
	}

	void RenderCamera::Rotate(Vector2 delta_xy)
	{
		//if(delta_xy.x > 0.001f || delta_xy.y > 0.001f)
		//{
		//	LOG_ERROR("DeltaXY {}, {}", delta_xy.x, delta_xy.y);
		//}

		yaw += delta_xy.x;
		pitch += delta_xy.y;

		if (yaw >= XM_2PI)
			yaw -= XM_2PI;
		if (yaw <= -XM_2PI)
			yaw += XM_2PI;
		pitch = std::clamp(pitch, s_MinPitchAngle, s_MaxPitchAngle);

		auto cosPitch = std::cos(pitch);
		auto newFront = Vector3{ cosPitch * std::sin(yaw), std::sin(pitch),  cosPitch * std::cos(yaw) };
		
		UpdateCameraFromFront(newFront);
	}

	void RenderCamera::MoveForward(float d)
	{
		pos += (front * d);
	}

	void RenderCamera::MoveRight(float d)
	{
		pos += (right * d);
	}

	void RenderCamera::MoveUp(float d)
	{
		pos += (up * d);
	}

	XMMATRIX RenderCamera::GetViewMatrix()
	{
		auto negPos = -pos;
		XMMATRIX mat = XMMatrixIdentity();
		mat.r[0] = XMLoadFloat3((XMFLOAT3*)&right);
		mat.r[1] = XMLoadFloat3((XMFLOAT3*)&up);
		mat.r[2] = XMLoadFloat3((XMFLOAT3*)&front);
		mat = XMMatrixTranspose(mat);
		Vector3 trans = { negPos.dotProduct(right), negPos.dotProduct(up), negPos.dotProduct(front) };
		mat.r[3] += XMLoadFloat3((XMFLOAT3*)&(trans));

		return mat;
	}

	XMMATRIX RenderCamera::GetProjMatrix()
	{
		return XMMatrixPerspectiveFovLH(fov, aspectRatio, znear, zfar);
	}

	void RenderCamera::UpdateCameraFromFront(Vector3 newFront)
	{
		front = newFront.normalisedCopy();
		right = Y.crossProduct(front).normalisedCopy();
		up = front.crossProduct(right).normalisedCopy();
	}

}
