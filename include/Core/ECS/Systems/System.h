#pragma once
#include "vec2.hpp"
#include "Application/Application.h"
#include <bitset>
#include <set>

#include "Core/ECS/ECSData.h"
#include <vector>

namespace Core::ECS
{
	class System
	{
	public:
		System() = default;
		virtual ~System() = default;
		virtual void RegisterInterestedComponents() = 0;
		virtual void BeginSystem(){}
		virtual void OnComponentAdded(const std::uint32_t entityID){}
		virtual void OnComponentRemoved(const std::uint32_t entityID){}
		virtual void UpdateSystem(const float deltaTime) = 0;
		virtual void EndSystem(){}
		virtual void CleanupSystem(){}

	protected:
		DoDebug(bool m_shouldRunOnlyWhilePlaying = false);
		std::bitset<MAX_COMPONENT_TYPES> m_systemBitSet = std::bitset<MAX_COMPONENT_TYPES>();
		friend class ECSManager;
	};
}
