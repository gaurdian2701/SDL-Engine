#pragma once
#include <cstdint>
#include <vector>
#include "ISparseSet.h"
#include "Core/ECS/ECSData.h"

namespace Core::ECS
{
	template<typename ComponentTypeUsedBySparseSet>
	class SparseSet : public ISparseSet
	{
	public:
		SparseSet(const std::uint32_t someMaxEntityCount) : m_maxEntityCount(someMaxEntityCount)
		{
			ReserveSpaceForEntities(someMaxEntityCount);
		}

		~SparseSet() override
		{
			m_sparseEntityArray.clear();
			m_denseEntityArray.clear();
			m_denseComponentArray.clear();
		}

		void AddComponentToEntity(const std::uint32_t entityID, ComponentTypeUsedBySparseSet&& component)
		{
			if (m_sparseEntityArray.size()-1 < entityID)
			{
				//Resize sparse array to accomodate new elements, and
				//reserve space for future allocations
				m_sparseEntityArray.resize(entityID);
				m_sparseEntityArray.reserve(m_maxEntityCount / 2);
				m_denseEntityArray.reserve(m_maxEntityCount / 2);
				m_denseComponentArray.reserve(m_maxEntityCount / 2);
			}

			if (m_sparseEntityArray[entityID] != INVALID_ENTITY_ID)
			{
				return;
			}

			m_denseEntityArray.push_back(entityID);
			m_denseComponentArray.push_back(std::forward<ComponentTypeUsedBySparseSet>(component));
			m_sparseEntityArray[entityID] = static_cast<std::uint32_t>(m_denseEntityArray.size()-1);
		}

		void RemoveComponentFromEntity(const std::uint32_t entityID) override
		{
			if (m_sparseEntityArray[entityID] == INVALID_ENTITY_ID)
			{
				return;
			}
			const auto swappableLastEntityIndex = m_denseEntityArray.back();

			//Swap component to be removed with the last element in dense array
			std::swap(m_denseEntityArray[m_sparseEntityArray[entityID]], m_denseEntityArray.back());
			std::swap(m_denseComponentArray[m_sparseEntityArray[entityID]], m_denseComponentArray.back());
			std::swap(m_sparseEntityArray[swappableLastEntityIndex], m_sparseEntityArray[entityID]);

			m_denseEntityArray.pop_back();
			m_denseComponentArray.pop_back();
			m_sparseEntityArray[entityID] = INVALID_ENTITY_ID;
		}

		std::vector<std::uint32_t>& GetSparseEntityArray() override
		{
			return m_sparseEntityArray;
		}

		std::vector<std::uint32_t>& GetDenseEntityArray() override
		{
			return m_denseEntityArray;
		}

		std::vector<ComponentTypeUsedBySparseSet>& GetDenseComponentArray()
		{
			return m_denseComponentArray;
		}

		void ReserveSpaceForEntities(std::uint32_t someEntityCount)
		{
			m_sparseEntityArray.resize(someEntityCount, INVALID_ENTITY_ID);
			m_sparseEntityArray.reserve(someEntityCount * 0.5f);
			m_denseEntityArray.reserve(someEntityCount);
			m_denseComponentArray.reserve(someEntityCount);
		}

		void ClearSparseSet()
		{
			m_sparseEntityArray.clear();
			m_denseEntityArray.clear();
			m_denseComponentArray.clear();

			m_sparseEntityArray.resize(m_maxEntityCount);
			m_denseEntityArray.resize(m_maxEntityCount);
			m_denseComponentArray.resize(m_maxEntityCount);
		}

	private:
		std::uint32_t m_maxEntityCount;

		std::vector<std::uint32_t> m_sparseEntityArray;
		std::vector<std::uint32_t> m_denseEntityArray = std::vector<uint32_t>(1, 0);
		std::vector<ComponentTypeUsedBySparseSet> m_denseComponentArray =
			std::vector<ComponentTypeUsedBySparseSet>(1, ComponentTypeUsedBySparseSet());
	};
}
