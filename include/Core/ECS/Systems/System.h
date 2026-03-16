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
		virtual void ProcessInitializationQueue() = 0;
		virtual void UpdateSystem(const float deltaTime)
		{
			ProcessInitializationQueue();
			m_initializationQueue.clear();
		}
		virtual void EndSystem(){}
		void CleanupSystem()
		{
			m_initializationQueue.clear();
		}

	protected:
		std::bitset<MAX_COMPONENT_TYPES> m_systemBitSet = std::bitset<MAX_COMPONENT_TYPES>();
		std::set<uint32_t> m_initializationQueue = std::set<uint32_t>();
		friend class ECSManager;
	};
}
