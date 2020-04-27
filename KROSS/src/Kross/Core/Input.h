#pragma once

#include "Kross/Core/Core.h"
#include "Kross/Core/KeyCodes.h"
#include <glm/glm.hpp>

namespace Kross {

	class KAPI Input
	{
	public:
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		inline static bool IsKeyReleased(int keycode) { return s_Instance->IsKeyReleasedImpl(keycode); }

		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
		inline static const glm::dvec2& GetMousePos() { return s_Instance->GetMousePosImpl(); }
		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }

		inline static void SetMousePos(const glm::dvec2& pos) { s_Instance->SetMousePosImpl(pos); }

		inline static void HideCursor() { s_Instance->HideCursorImpl(); }
		inline static void ShowCursor() { s_Instance->ShowCursorImpl(); }
	protected:
		glm::dvec2 mouse_position = { 0.0, 0.0 };
		bool Keys[KROSS_KEY_LAST] = { false };

		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsKeyReleasedImpl(int keycode) = 0;

		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual const glm::dvec2& GetMousePosImpl() = 0;

		virtual void SetMousePosImpl(const glm::dvec2& pos) = 0;

		virtual void HideCursorImpl() = 0;
		virtual void ShowCursorImpl() = 0;
	private:
		static Input* s_Instance;

	};
}
