#pragma once

#include "deco_model.h"

#include <memory>

namespace Deco
{
	struct Transform2dComponent
	{
		glm::vec2 m_translation{}; // position offset
		glm::vec2 m_scale{1.0f, 1.0f};
		float m_rotation;

		glm::mat2 mat2()
		{
			const float s = glm::sin(m_rotation);
			const float c = glm::cos(m_rotation);
			glm::mat2 rotation_matrix{ {c,s}, {-s,c} };

			glm::mat2 scale_mat{ {m_scale.x, 0.0f}, {0.0f, m_scale.y} };
			return rotation_matrix * scale_mat;
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
		Transform2dComponent m_transform2d{};

	private:
		DecoGameObject(GameObjectID object_id) : m_id(object_id) {}

	private:
		GameObjectID m_id;
	};
}