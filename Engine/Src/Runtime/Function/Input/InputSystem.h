#pragma once

#include "Function/Global/RuntimeGlobalContext.h"
#include "Core/Math/Vector4.h"
#include "Core/Math/Vector2i.h"
#include "Function/Event/EventHeaders.h"
#include "Function/Util/GameTimer.h"

namespace photon
{
	enum class GameCommand : unsigned int
	{
		forward = 1 << 0,                 // W
		backward = 1 << 1,                 // S
		left = 1 << 2,                 // A
		right = 1 << 3,                 // D
		jump = 1 << 4,                 // GameMode: SPACE / EditorMode: q
		squat = 1 << 5,                 // GameMode: ctrl / EditorMode: e
		sprint = 1 << 6,                 // LEFT SHIFT
		fire = 1 << 7,                 // not implemented yet
		gameMode = 1 << 8,              // EditorMode
		invalid = (unsigned int)(1 << 31) // lost focus
	};

	extern unsigned int k_ComplementCommand;

	class InputSystem
	{

	public:
		void OnKeyDown(KeyDownEvent& keydown);
		void OnKeyUp(KeyUpEvent& keyup);
		void OnCursorPos(MouseMoveEvent& mouseMove);

		void Initialize();
		void Tick(GameTimer& gt);
		void Clear();

		Vector2i cursorDelta = { 0, 0 };

		Vector2 cursorDeltaYawPitch { 0.0f, 0.0f };

		float moveSpeed = 3.0f;
		Vector2 rotateSpeed = { 3.0f, 3.0f };

		void         ResetGameCommand() { m_GameCommand = 0; }
		unsigned int GetGameCommand() const { return m_GameCommand; }

	private:
		void OnKeyDownInGameMode(KeyDownEvent& keydown);
		void OnKeyUpInGameMode(KeyUpEvent& keyup);

		void CalculateCursorDeltaAngles(GameTimer& gt);

		unsigned int m_GameCommand{ 0 };

		Vector2i m_LastCursor{ 0, 0 };
	};
} // namespace Piccolo
