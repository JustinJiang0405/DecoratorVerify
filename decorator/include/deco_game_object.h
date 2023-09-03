#pragma once

#include "deco_model.h"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace Deco
{
	struct TransformComponent
	{
		glm::vec3 m_translation{}; // position offset
		glm::vec3 m_scale{1.0f, 1.0f, 1.0f};
		glm::vec3 m_rotation{};

		// Matrix corresponds to tranlate * Ry * Rx * Rz * scale transormation
		// Rotation convention uses tait-bryan angles with axis order Y(1), X(2), Z(3)
		glm::mat4 mat4()
		{
			const float c3 = glm::cos(m_rotation.z);
			const float s3 = glm::sin(m_rotation.z);
			const float c2 = glm::cos(m_rotation.x);
			const float s2 = glm::sin(m_rotation.x);
			const float c1 = glm::cos(m_rotation.y);
			const float s1 = glm::sin(m_rotation.y);
			return glm::mat4{
				{
					m_scale.x * (c1 * c3 + s1 * s2 * s3),
					m_scale.x * (c2 * s3),
					m_scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					m_scale.y* (c3 * s1 * s2 - c1 * s3),
					m_scale.y* (c2 * c3),
					m_scale.y* (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					m_scale.z * (c2 * s1),
					m_scale.z * (-s2),
					m_scale.z * (c1 * c2),
					0.0f,
				},
				{m_translation.x, m_translation.y, m_translation.z, 1.0f} };
		}
	};

	class DecoGameObject
	{
	public:
		using GameObjectID = unsigned int;

		DecoGameObject(const DecoGameObject&) = delete;
		DecoGameObject& operator=(const DecoGameObject&) = delete;
		DecoGameObject(DecoGameObject&&) = default;
		DecoGameObject& operator=(DecoGameObject&&) = default;

		static DecoGameObject createGameObject()
		{
			static GameObjectID current_id = 0;
			return DecoGameObject{ current_id++ };
		}

		GameObjectID getId() { return m_id; }

	public:
		std::shared_ptr<DecoModel> m_model{};
		glm::vec3 m_color{};
		TransformComponent m_transform{};

	private:
		DecoGameObject(GameObjectID object_id) : m_id(object_id) {}

	private:
		GameObjectID m_id;
	};
}