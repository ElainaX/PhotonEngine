#include "InputSystem.h"

#include "Function/Render/WindowSystem.h"
#include "Function/Render/RenderSystem.h"
#include "Core/Math/MathFunction.h"

namespace photon 
{
	unsigned int k_ComplementCommand = 0xFFFFFFFF;

	void InputSystem::OnKeyDown(KeyDownEvent& keydown)
	{
		if(keydown.GetVirtualKey() == VirtualKey::F)
		{
			m_GameCommand ^= (unsigned int)GameCommand::gameMode;
			g_RuntimeGlobalContext.windowSystem->SetFocusMode(m_GameCommand & (unsigned int)GameCommand::gameMode);
			return;
		}

		if((unsigned int)GameCommand::gameMode & GetGameCommand())
		{
			this->OnKeyDownInGameMode(keydown);
		}
	}

	void InputSystem::OnKeyUp(KeyUpEvent& keyup)
	{
		if ((unsigned int)GameCommand::gameMode & GetGameCommand())
		{
			this->OnKeyUpInGameMode(keyup);
		}
	}

	void InputSystem::OnCursorPos(MouseMoveEvent& mouseMove)
	{
		auto mousePos = WindowUtil::ClientToScreen(g_RuntimeGlobalContext.windowSystem->GetHwnd(), mouseMove.GetMousePos());
		if(g_RuntimeGlobalContext.windowSystem->GetFocusMode())
		{
			cursorDelta += mousePos - m_LastCursor;
		}

		m_LastCursor = mousePos;
	}

	void InputSystem::Initialize()
	{
		auto windowSystem = g_RuntimeGlobalContext.windowSystem;
		windowSystem->RegisterOnKeyDownCallback([this](KeyDownEvent& e) 
			{
				this->OnKeyDown(e);
			});
		windowSystem->RegisterOnKeyUpCallback([this](KeyUpEvent& e)
			{
				this->OnKeyUp(e);
			});
		windowSystem->RegisterOnMouseMoveCallback([this](MouseMoveEvent& e)
			{
				this->OnCursorPos(e);
			});
	}

	void InputSystem::Tick(GameTimer& gt)
	{
		CalculateCursorDeltaAngles(gt);
		Clear();

		if(g_RuntimeGlobalContext.windowSystem->GetFocusMode())
		{
			auto renderCamera = g_RuntimeGlobalContext.renderSystem->GetRenderCamera();
			if (m_GameCommand & (unsigned int)GameCommand::forward)
			{
				//LOG_ERROR("MoveForward {}", moveSpeed * gt.DeltaTime());
				renderCamera->MoveForward(moveSpeed * gt.DeltaTime());
			}
			if (m_GameCommand & (unsigned int)GameCommand::backward)
			{
				//LOG_ERROR("MoveBackward {}", moveSpeed * gt.DeltaTime());
				renderCamera->MoveForward(-moveSpeed * gt.DeltaTime());
			}
			if (m_GameCommand & (unsigned int)GameCommand::left)
			{
				//LOG_ERROR("MoveLeft {}", moveSpeed * gt.DeltaTime());
				renderCamera->MoveRight(-moveSpeed * gt.DeltaTime());
			}
			if (m_GameCommand & (unsigned int)GameCommand::right)
			{
				//LOG_ERROR("MoveRight {}", moveSpeed * gt.DeltaTime());
				renderCamera->MoveRight(moveSpeed * gt.DeltaTime());
			}
			if (m_GameCommand & (unsigned int)GameCommand::jump)
			{
				//LOG_ERROR("MoveUp {}", moveSpeed * gt.DeltaTime());
				renderCamera->MoveUp(moveSpeed * gt.DeltaTime());
			}
			if (m_GameCommand & (unsigned int)GameCommand::squat)
			{
				//LOG_ERROR("MoveDown {}", moveSpeed * gt.DeltaTime());
				renderCamera->MoveUp(- moveSpeed * gt.DeltaTime());
			}
		}

	}

	void InputSystem::Clear()
	{
		cursorDelta = { 0, 0 };
	}

	void InputSystem::OnKeyDownInGameMode(KeyDownEvent& keydown)
	{
		switch(keydown.GetVirtualKey())
		{
		case VirtualKey::W:
			m_GameCommand |= (unsigned int)GameCommand::forward;
			break;
		case VirtualKey::A:
			m_GameCommand |= (unsigned int)GameCommand::left;
			break;
		case VirtualKey::S:
			m_GameCommand |= (unsigned int)GameCommand::backward;
			break;
		case VirtualKey::D:
			m_GameCommand |= (unsigned int)GameCommand::right;
			break;
		case VirtualKey::Q:
			m_GameCommand |= (unsigned int)GameCommand::jump;
			break;
		case VirtualKey::E:
			m_GameCommand |= (unsigned int)GameCommand::squat;
			break;
		}


	}

	void InputSystem::OnKeyUpInGameMode(KeyUpEvent& keyup)
	{
		switch (keyup.GetVirtualKey())
		{
		case VirtualKey::W:
			m_GameCommand &= (k_ComplementCommand ^ (unsigned int)GameCommand::forward);
			break;
		case VirtualKey::A:
			m_GameCommand &= (k_ComplementCommand ^ (unsigned int)GameCommand::left);
			break;
		case VirtualKey::S:
			m_GameCommand &= (k_ComplementCommand ^ (unsigned int)GameCommand::backward);
			break;
		case VirtualKey::D:
			m_GameCommand &= (k_ComplementCommand ^ (unsigned int)GameCommand::right);
			break;
		case VirtualKey::Q:
			m_GameCommand &= (k_ComplementCommand ^ (unsigned int)GameCommand::jump);
			break;
		case VirtualKey::E:
			m_GameCommand &= (k_ComplementCommand ^ (unsigned int)GameCommand::squat);
			break;
		}
	}

	void InputSystem::CalculateCursorDeltaAngles(GameTimer& gt)
	{
		Vector2i windowSize = g_RuntimeGlobalContext.windowSystem->GetClientWidthAndHeight();
		
		auto renderCamera = g_RuntimeGlobalContext.renderSystem->GetRenderCamera();
		Vector2 fov = renderCamera->GetFov();

		Vector2 speed = rotateSpeed * gt.DeltaTime();
		Vector2 cursorDeltaRad = speed * Vector2{ (float)cursorDelta.x, (float)cursorDelta.y };

		cursorDeltaYawPitch.x = cursorDeltaRad.x;
		cursorDeltaYawPitch.y = -cursorDeltaRad.y;


		
		if (g_RuntimeGlobalContext.windowSystem->GetFocusMode())
		{
			//LOG_ERROR("cursorDeltaYawPitch {},{}", cursorDeltaYawPitch.x, cursorDeltaYawPitch.y);
			renderCamera->Rotate(cursorDeltaYawPitch);
			m_LastCursor = WindowUtil::SetMousePosToWndCenter(g_RuntimeGlobalContext.windowSystem->GetHwnd());
		}
	}

}