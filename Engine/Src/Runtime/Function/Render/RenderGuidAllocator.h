#pragma once

#include <unordered_map>
#include <queue>
#include <optional>
#include <limits>

namespace photon 
{
	// 不使用Guid 0
	static const uint32_t s_InvalidGuid = 0xFFFFFFFF;

	template<typename T>
	class GuidAllocator
	{
	public:
		static bool IsValidGuid(uint32_t guid) { return s_InvalidGuid != guid; }

		uint32_t AllocGuid(const T& e)
		{
			auto find_it = m_Element2GuidMap.find(e);
			if(find_it != m_Element2GuidMap.end())
			{
				return find_it->second;
			}
			
			uint32_t newGuid = 0;
			if(!m_FreeGuids.empty())
			{
				newGuid = m_FreeGuids.front();
				m_FreeGuids.pop();
			}
			else 
			{
				newGuid = ++m_LastAllocGuid;
			}

			m_Element2GuidMap.insert(std::make_pair(e, newGuid));
			m_Guid2ElementMap.insert(std::make_pair(newGuid, e));

			return newGuid;
		}

		uint32_t GetGuid(const T& e)
		{
			auto find_it = m_Element2GuidMap.find(e);
			if(find_it != m_Element2GuidMap.end())
			{
				return find_it->second;
			}
			return s_InvalidGuid;
		}

		bool IsElementGuidAllocated(const T& e) const 
		{
			return m_Element2GuidMap.find(e) != m_Element2GuidMap.end();
		}

		bool IsGuidAllocated(uint32_t guid) const
		{
			return m_Guid2ElementMap.find(guid) != m_Guid2ElementMap.end();
		}


		std::optional<std::reference_wrapper<T>> GetElement(uint32_t guid)
		{
			auto find_it = m_Guid2ElementMap.find(guid);
			if (find_it != m_Guid2ElementMap.end())
			{
				return find_it->second;
			}
			return std::nullopt;
		}

		void FreeGuid(uint32_t guid)
		{
			auto find_it = m_Guid2ElementMap.find(guid);
			if (find_it != m_Guid2ElementMap.end())
			{
				auto& e = find_it->second;
				m_Element2GuidMap.erase(e);
				m_Guid2ElementMap.erase(guid);
				m_FreeGuids.push(guid);
			}
		}

		void FreeElementGuid(const T& e)
		{
			auto find_it = m_Element2GuidMap.find(e);
			if (find_it != m_Element2GuidMap.end())
			{
				auto guid = find_it->second;
				m_Guid2ElementMap.erase(guid);
				m_Element2GuidMap.erase(e);
				m_FreeGuids.push(guid);
			}
		}

		std::vector<uint32_t> GetAllocatedGuids() const 
		{
			std::vector<uint32_t> allocatedGuids;
			allocatedGuids.reserve(m_Guid2ElementMap.size());
			for(auto& e : m_Guid2ElementMap)
			{
				allocatedGuids.emplace_back(e.first);
			}
			return allocatedGuids;
		}

		void Clear()
		{
			m_Element2GuidMap.clear();
			m_Guid2ElementMap.clear();
			m_FreeGuids.clear();
			m_LastAllocGuid = 0;
		}

	private:
		// 被使用后返还的Guid存在m_FreeGuids中
		std::queue<uint32_t> m_FreeGuids;
		std::unordered_map<T, uint32_t> m_Element2GuidMap;
		std::unordered_map<uint32_t, T> m_Guid2ElementMap;
		uint32_t m_LastAllocGuid = s_InvalidGuid;
	};
}