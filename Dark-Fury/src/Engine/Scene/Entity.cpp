#include "engpch.h"
#include "Engine/Scene/Entity.hpp"

namespace Engine {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

}