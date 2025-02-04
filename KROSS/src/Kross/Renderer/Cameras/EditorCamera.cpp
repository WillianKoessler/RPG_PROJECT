#include <Kross_pch.h>
#include "EditorCamera.h"

#include "Kross/Core/Input.h"
#include "Kross/Events/KeyCodes.h"
#include "Kross/Events/MouseButtonCodes.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Kross::Camera {

	Editor::Editor(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip), m_ProjMat(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		UpdateView();
	}
	void Editor::UpdateProj()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_ProjMat = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}
	void Editor::UpdateView()
	{
		// m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
		m_Position = CalculatePosition();
		static constexpr glm::mat4 mat4(1.0f);
		m_ViewMat = glm::inverse(glm::translate(mat4, m_Position) * glm::toMat4(GetOrientation()));
	}
	std::pair<float, float> Editor::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, phi<float>()); // max = 3.1.61903398874989484920721002966692491f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, phi<float>()); // max = 3.1.61903398874989484920721002966692491f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}
	float Editor::RotationSpeed() const
	{
		return glm::pi<float>();
	}
	float Editor::ZoomSpeed() const
	{
		float distance = m_Distance * 0.5f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}
	void Editor::OnUpdate(double ts)
	{
		UpdateView();
		m_bLeftShift = m_bLeftCtrl = m_bMouseMiddle = false;
	}
	void Editor::OnEvent(Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(KROSS_BIND_EVENT_FN(Editor::OnMouseScroll));
		dispatcher.Dispatch<MouseMovedEvent>(KROSS_BIND_EVENT_FN(Editor::OnMouseMoved));
	}
	bool Editor::OnMouseMoved(MouseMovedEvent &e)
	{
		m_MousePosition = { e.GetX(), e.GetY() };
		m_Delta = (m_MousePosition - m_InitialMousePosition) * 0.003f;
		m_InitialMousePosition = m_MousePosition;

		//if (Input::IsMouseButtonHeld(MouseButton::Middle))
		if (Input::IsKeyHeld(Key::Shift)) MousePan(m_Delta);
		else if (Input::IsKeyHeld(Key::Control)) MouseZoom(m_Delta.y);
		else if (Input::IsKeyHeld(Key::Alt)) MouseRotate(m_Delta);

		return false;
	}
	bool Editor::OnMouseScroll(MouseScrolledEvent &e)
	{
		float delta = e.GetYOffSet() * 0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}
	void Editor::MousePan(const glm::vec2 &delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint += -GetRightDir() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDir() * delta.y * ySpeed * m_Distance;
	}
	void Editor::MouseRotate(const glm::vec2 &delta)
	{
		float yawSign = GetUpDir().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}
	void Editor::MouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		m_Distance = std::max(m_Distance, phi<float>() * 0.1f); // Maximum zoom = 0.161903398874989484920721002966692491f
	}
	glm::vec3 Editor::GetUpDir() const { return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f)); }
	glm::vec3 Editor::GetRightDir() const { return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f)); }
	glm::vec3 Editor::GetForwardDir() const { return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f)); }
	glm::vec3 Editor::CalculatePosition() const { return m_FocalPoint - GetForwardDir() * m_Distance; }
	glm::quat Editor::GetOrientation() const { return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f)); }

}
