#pragma once

#include "deco_model.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace Deco
{
	struct TransformComponent
	{
		glm::vec3 m_translation{}; // position offset
		glm::vec3 m_scale{1.0f, 1.0f, 1.0f};
		glm::vec3 m_rotation{};

		// Matrix corresponds to tranlate * Ry * Rx * Rz * scale transormation
		// Rotation convention uses tait-bryan angles with axis order Y(1), X(2), Z(3)
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	class DecoGameObject
	{
	public:
		using GameObjectID = unsigned int;
		using Map = std::unordered_map<GameObjectID, DecoGameObject>;

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