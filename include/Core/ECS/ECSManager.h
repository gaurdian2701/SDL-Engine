#pragma once
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <bitset>
#include <iostream>
#include "Components/ComponentFactory.h"
#include "SparseSet/SparseSet.h"
#include "ECSData.h"
#include "Systems/ParticleSystem.h"
#include "Systems/System.h"

namespace Core::ECS
{
    class ECSManager
    {
    public:
        ECSManager()
        {
            m_componentRemovalHandles.resize(MAX_COMPONENT_TYPES);
        }

        ~ECSManager() = default;

        static ECSManager& GetInstance()
        {
            static ECSManager* instance = new ECSManager();
            return *instance;
        }

        void InitializeManager(uint32_t someMaxEntities);
        void BeginSystems();
        void UpdateManager(const float deltaTime);
        void CleanupManager();

        std::vector<void(*)(ECSManager &, const std::uint32_t)>& GetComponentRemovalHandlesArray();

        template <typename T>
        SparseSet<T>* GetComponentPool()
        {
            return static_cast<SparseSet<T>*>(m_componentPoolMap[std::type_index(typeid(T))]);
        }

        template<typename T>
        void RegisterComponent()
        {
            m_componentFactory.RegisterComponent<T>();
            m_componentPoolMap[std::type_index(typeid(T))] = new SparseSet<T>(m_maxEntities);
            GetGeneratedComponentTypeIndex<T>(*this);
        }

        template<typename FirstComponentType, typename... OtherComponentTypes>
        void RegisterInterestedComponentsForSystem(System* someSystem)
        {
            std::bitset<MAX_COMPONENT_TYPES> componentBitSetForSystem = std::bitset<MAX_COMPONENT_TYPES>(0);
            componentBitSetForSystem.set(GetGeneratedComponentTypeIndex<FirstComponentType>(*this), true);
            ([&]
            {
                componentBitSetForSystem.set(GetGeneratedComponentTypeIndex<OtherComponentTypes>(*this), true);
            }(), ...);

            someSystem->m_systemBitSet = componentBitSetForSystem;
        }

        template<typename T>
        void AddComponent(const std::uint32_t someEntityID)
        {
            dynamic_cast<SparseSet<T>*>(m_componentPoolMap[std::type_index(typeid(T))])
            ->AddComponentToEntity(someEntityID, std::forward<T>(m_componentFactory.CreateComponent<T>()));

            m_entityBitSetMap[someEntityID].set(GetGeneratedComponentTypeIndex<T>(*this), true);
            std::bitset<MAX_COMPONENT_TYPES> componentsBitSetForEntity = m_entityBitSetMap[someEntityID];

            for (auto system : m_SystemsList)
            {
                if ((componentsBitSetForEntity & system->m_systemBitSet) == system->m_systemBitSet)
                {
                    system->m_initializationQueue.insert(someEntityID);
                }
            }
        }

        template<typename T>
        void RemoveComponent(const std::uint32_t someEntityID)
        {
            dynamic_cast<SparseSet<T>*>(m_componentPoolMap[std::type_index(typeid(T))])
            ->RemoveComponentFromEntity(someEntityID);
            m_entityBitSetMap[someEntityID].set(GetGeneratedComponentTypeIndex<T>(*this), false);
        }

        template<typename T>
        T* GetComponent(std::uint32_t someEntityID)
        {
            auto componentPool = GetComponentPool<T>();
            uint32_t denseIndex = componentPool->GetSparseEntityArray()[someEntityID];

            if (denseIndex == INVALID_ENTITY_ID)
            {
                return nullptr;
            }
            return &componentPool->GetDenseComponentArray()[denseIndex];
        }

        template<typename FirstComponentType, typename ... OtherComponentTypes>
        std::vector<std::uint32_t>& GetSmallestDenseEntityArray()
        {
            //By default, smallest entity array is the entity array of FirstComponentType
            std::uint32_t smallestEntityArraySize = UINT32_MAX;
            std::type_index smallestEntityArrayTypeIndex = typeid(FirstComponentType);
            std::vector<std::uint32_t>* smallestEntityArray = &m_componentPoolMap[smallestEntityArrayTypeIndex]->GetDenseEntityArray();

            //If OtherComponentTypes has a smaller entity array, then make that smallest, etc.
            ([&]
            {
                auto& denseEntityArray = m_componentPoolMap[std::type_index(typeid(OtherComponentTypes))]->GetDenseEntityArray();
                if (const int denseEntityArraySize = denseEntityArray.size(); denseEntityArraySize < smallestEntityArraySize && denseEntityArraySize > 0)
                {
                    smallestEntityArraySize = denseEntityArraySize;
                    smallestEntityArrayTypeIndex = typeid(OtherComponentTypes);
                    smallestEntityArray = &denseEntityArray;
                }
            }(), ...);

            return *smallestEntityArray;
        }

        template<typename FirstComponentType, typename ... OtherComponentTypes>
        std::tuple<std::vector<FirstComponentType>&, std::vector<OtherComponentTypes>& ...> QueryDenseComponentArrays()
        {
            return std::forward_as_tuple(
                GetComponentPool<FirstComponentType>()->GetDenseComponentArray(),
                GetComponentPool<OtherComponentTypes>()->GetDenseComponentArray() ...);
        }

        template<typename FirstComponentType, typename ... OtherComponentTypes>
        auto QuerySparseEntityArrays()
        {
            return std::forward_as_tuple(
                GetComponentPool<FirstComponentType>()->GetSparseEntityArray(),
                GetComponentPool<OtherComponentTypes>()->GetSparseEntityArray() ...);
        }

        //Query Dense Component Arrays with the help of Component Types
        template<typename FirstComponentType, typename ... OtherComponentTypes, typename Functor>
        void ForEachUsingComponents(const Functor& someFunctor)
        {
            const auto& entities = GetSmallestDenseEntityArray<FirstComponentType, OtherComponentTypes ...>();

            //Query dense component arrays and pass them into the functor
            for (uint32_t index = 1; index < entities.size(); ++index)
            {
                uint32_t entityID = entities[index];
                someFunctor(GetComponent<FirstComponentType>(entityID), GetComponent<OtherComponentTypes>(entityID) ...);
            }
        }

        template<typename FirstComponentType, typename ... OtherComponentTypes, typename Functor>
        void BreakableForEachUsingComponentsWithEntityID(const Functor& someFunctor)
        {
            const auto& entities = GetSmallestDenseEntityArray<FirstComponentType, OtherComponentTypes ...>();
            bool breakInitiated = false;

            //Query dense component arrays and pass them into the functor
            for (uint32_t index = 1; index < entities.size(); ++index)
            {
                uint32_t entityID = entities[index];
                someFunctor(breakInitiated, entityID, GetComponent<FirstComponentType>(entityID), GetComponent<OtherComponentTypes>(entityID) ...);
                if (breakInitiated)
                {
                    break;
                }
            }
        }

        template<typename FirstComponentType, typename ... OtherComponentTypes, typename Functor>
        void ForEachUsingComponentsWithEntityID(const Functor& someFunctor)
        {
            const auto& entities = GetSmallestDenseEntityArray<FirstComponentType, OtherComponentTypes ...>();

            //Query dense component arrays and pass them into the functor
            for (uint32_t index = 1; index < entities.size(); ++index)
            {
                uint32_t entityID = entities[index];
                someFunctor(entityID, GetComponent<FirstComponentType>(entityID), GetComponent<OtherComponentTypes>(entityID) ...);
            }
        }

        template<typename FirstComponentType, typename ... OtherComponentTypes, typename Functor>
        void ForEachUsingEntities(const std::set<uint32_t>& someEntityIDs, const Functor& someFunctor)
        {
            for (auto entityID : someEntityIDs)
            {
                someFunctor(GetComponent<FirstComponentType>(entityID), GetComponent<OtherComponentTypes>(entityID) ...);
            }
        }

        template<typename FirstComponentType, typename ... OtherComponentTypes, typename Functor>
        void ForEachUsingEntitiesWithEntityID(const std::set<uint32_t>& someEntityIDs, const Functor& someFunctor)
        {
            for (auto entityID : someEntityIDs)
            {
                someFunctor(entityID, GetComponent<FirstComponentType>(entityID), GetComponent<OtherComponentTypes>(entityID) ...);
            }
        }

        template<typename T>
        [[nodiscard]] static bool RegisterComponentRemovalFunctionHandle(ECSManager& someManager,
            std::size_t componentTypeIndex)
        {
            //If a gameobject with components attached gets deleted at runtime, I have to
            //untrack it's corresponding components in the ECS. Instead of manually having to
            //call RemoveComponent() for every component in the destructor, I thought of automating it
            //by having an array of handles to those corresponding RemoveComponent() functions,
            //stored in a map with the ComponentTypeIndex as the key. This is solely for that purpose.

            someManager.GetComponentRemovalHandlesArray()[componentTypeIndex] =
                [](ECSManager& manager, const std::uint32_t someEntityID)
                {
                    manager.RemoveComponent<T>(someEntityID);
                };

            return true;
        }

        [[nodiscard]] static std::size_t GenerateIndex()
        {
            static std::size_t index = 0;
            return index++;
        }

        template<typename T>
        static std::size_t GetGeneratedComponentTypeIndex(ECSManager& someManager)
        {
            static std::size_t componentTypeIndex = GenerateIndex();
            static bool TryGenerateComponentRemovalFunctionHandle =
                RegisterComponentRemovalFunctionHandle<T>(someManager, componentTypeIndex);

            if (componentTypeIndex == MAX_COMPONENT_TYPES)
            {
                return INVALID_ENTITY_ID;
            }
            return componentTypeIndex;
        }

        std::uint32_t GenerateEntityID();
        void FreeEntityID(const std::uint32_t entityID);

    private:
        void CreateSystems();
        void InitializeSystems();

    private:
        std::uint32_t m_maxEntities = 0;

        std::vector<System*> m_SystemsList;
        std::vector<std::uint32_t> m_entityFreeList;
        std::vector<void(*)(ECSManager&, const std::uint32_t)> m_componentRemovalHandles;

        //TODO: Wrap sparse set in a class and have functions like HasComponent, TryGetComponent, etc.

        std::unordered_map<std::type_index, ISparseSet*> m_componentPoolMap;
        std::unordered_map<uint32_t, std::bitset<MAX_COMPONENT_TYPES>> m_entityBitSetMap = std::unordered_map<uint32_t, std::bitset<MAX_COMPONENT_TYPES>>();
        Components::ComponentFactory m_componentFactory;
    };
}
