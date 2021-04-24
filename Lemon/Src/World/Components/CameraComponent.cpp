#include "LemonPCH.h"
#include "CameraComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include "World/World.h"
#include "World/Entity.h"
#include "TransformComponent.h"
#include "Input/InputSystem.h"
#include <glm/gtx/compatibility.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>

namespace Lemon
{
    CameraComponent::CameraComponent()
    {
        RecalculateProjection();
    }

    void CameraComponent::SetOrthographic(float size, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Orthographic;
        m_OrthographicSize = size;
        m_OrthographicNear = nearClip;
        m_OrthographicFar = farClip;
		RecalculateProjection();
    }
    void CameraComponent::SetPerspective(float horizontalFOV, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOVHorizontalRadian = horizontalFOV;
        m_PerspectiveNear = nearClip;
        m_PerspectiveFar = farClip;
        RecalculateProjection();
    }
        
    void CameraComponent::SetViewportSize(uint32_t width, uint32_t height)
    {
		m_AspectRatio = (float)width / (float)height;
		m_ViewportSizeX = width;
		m_ViewportSizeY = height;

		RecalculateProjection();
    }

    
    void CameraComponent::RecalculateProjection()
    {
        if(m_ProjectionType == ProjectionType::Perspective)
        {
			// caution : for Direct3D the perspective need in LR and Zero to One 
            m_ProjectionMatrix = glm::perspectiveLH_ZO(GetFovVerticalRadian(), m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
        }
        else
        {
            float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoBottom = -m_OrthographicSize * 0.5f;
			float orthoTop = m_OrthographicSize * 0.5f;

            m_ProjectionMatrix = glm::ortho(orthoLeft, orthoRight,
                orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
        }
    }

	float CameraComponent::GetFovVerticalRadian() const
	{
		return 2.0f * atan(tan(m_PerspectiveFOVHorizontalRadian / 2.0f) * (m_ViewportSizeY / m_ViewportSizeX));
	}

	const glm::mat4& CameraComponent::GetViewMatrix() const
	{
		if (m_Entity)
		{
			return glm::inverse(m_Entity.GetComponent<TransformComponent>().GetTransform());
		}
		return glm::mat4(1.0f);
	}


	void CameraComponent::ProcessInputSystem(float deltaTime)
	{
		if (!m_Entity.GetWorld() || !m_Entity.GetWorld()->GetEngine())
			return;
		InputSystem* inputSystem = m_Entity.GetWorld()->GetEngine()->GetSystem<InputSystem>();
		if (!inputSystem)
			return;
		static const float mouseSensitivity = 0.13f;
		static const float movementSpeedMax = 40.0f;
		static const float movementAcceration = 0.8f;
		static const float movementDrag = 0.08f;

		if (inputSystem->GetKey(KeyCode::Mouse_Left))
		{
			// Get Mouse delta
			glm::vec2 mouseDelta = inputSystem->GetMouseDelta() * mouseSensitivity;

			// Compute rotation
			TransformComponent& transformComp = m_Entity.GetComponent<TransformComponent>();
			transformComp.Rotation += glm::vec3(mouseDelta.y, mouseDelta.x, 0);
			// Clamp rotation along the x-axis
			transformComp.Rotation.x = glm::clamp(transformComp.Rotation.x, -90.0f, 90.0f);

			// Keyboard movement
			glm::vec3 direction = { 0,0,0 };
			if (inputSystem->GetKey(KeyCode::W)) direction += transformComp.GetForwardVector();
			if (inputSystem->GetKey(KeyCode::S)) direction += transformComp.GetBackVector();
			if (inputSystem->GetKey(KeyCode::D)) direction += transformComp.GetRightVector();
			if (inputSystem->GetKey(KeyCode::A)) direction += transformComp.GetLeftVector();
			if (inputSystem->GetKey(KeyCode::Q)) direction += transformComp.GetUpVector();
			if (inputSystem->GetKey(KeyCode::E)) direction += transformComp.GetDownVector();

			// Compute speed
			m_MovementSpeed += direction * movementAcceration;
			m_MovementSpeed.x = glm::clamp(m_MovementSpeed.x, -movementSpeedMax, movementSpeedMax);
			m_MovementSpeed.y = glm::clamp(m_MovementSpeed.y, -movementSpeedMax, movementSpeedMax);
			m_MovementSpeed.z = glm::clamp(m_MovementSpeed.z, -movementSpeedMax, movementSpeedMax);

			// Apply movement drag
			m_MovementSpeed *= 1.0f - movementDrag;

			// Translate for as long as there is speed
			if (m_MovementSpeed != glm::vec3(0, 0, 0));
			{
				transformComp.Position += m_MovementSpeed * deltaTime;
			}
		}

	}
}
