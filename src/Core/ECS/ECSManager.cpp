#include "Core/ECS/ECSManager.h"
#include "Core/ECS/Systems/ParticleSystem.h"
#include "Core/ECS/Systems/PhysicsSystem.h"
#include "Core/ECS/Systems/RenderingSystem.h"
#include "Core/ECS/Systems/TransformSolverSystem.h"

std::vector<void(*)(Core::ECS::ECSManager &, const std::uint32_t)>&
Core::ECS::ECSManager::GetComponentRemovalHandlesArray()
{
	return m_componentRemovalHandles;
}

void Core::ECS::ECSManager::InitializeManager(uint32_t someMaxEntities)
{
	static bool hasInitialized = false;
	if (hasInitialized)
	{
		return;
	}
	m_maxEntities = someMaxEntities;
	CreateSystems();
	InitializeSystems();
	hasInitialized = true;
}

void Core::ECS::ECSManager::CreateSystems()
{
	m_SystemsList.push_back(new Systems::TransformSolverSystem());
	m_SystemsList.push_back(new Systems::ParticleSystem());
	m_SystemsList.push_back(new Systems::PhysicsSystem());
	m_SystemsList.push_back(new Systems::RenderingSystem());
}

void Core::ECS::ECSManager::InitializeSystems()
{
	for (auto system : m_SystemsList)
	{
		system->RegisterInterestedComponents();
	}
}

void Core::ECS::ECSManager::BeginSystems()
{
	for (auto& system : m_SystemsList)
	{
		system->BeginSystem();
	}
}

void Core::ECS::ECSManager::UpdateManager(const float deltaTime)
{
	for (auto& system : m_SystemsList)
	{
		system->UpdateSystem(deltaTime);
	}
}

void Core::ECS::ECSManager::CleanupManager()
{
	for (auto& system : m_SystemsList)
	{
		system->CleanupSystem();
	}
}

std::uint32_t Core::ECS::ECSManager::GenerateEntityID()
{
	static std::uint32_t nextEntityID = 1;

	if (!m_entityFreeList.empty())
	{
		std::uint32_t entityID = m_entityFreeList.back();
		m_entityFreeList.pop_back();
		return entityID;
	}

	return nextEntityID++;
}

void Core::ECS::ECSManager::FreeEntityID(const std::uint32_t entityID)
{
	if (std::find(m_entityFreeList.begin(), m_entityFreeList.end(), entityID) == m_entityFreeList.end())
	{
		m_entityFreeList.push_back(entityID);
	}
}


