#pragma once

#include <unordered_map>
#include <memory>
#include <string>

namespace photon
{
	class PassBlackboard
	{
	public:
		void Clear() { m_Data.clear(); }

		template<typename T>
		void Set(const std::string& key, std::shared_ptr<T> value)
		{
			m_Data[key] = value;
		}

		template<typename T>
		std::shared_ptr<T> Get(const std::string& key) const
		{
			auto it = m_Data.find(key);
			if (it == m_Data.end())
				return nullptr;
			return std::static_pointer_cast<T>(it->second);
		}


	private:
		std::unordered_map<std::string, std::shared_ptr<void>> m_Data;

	};
}