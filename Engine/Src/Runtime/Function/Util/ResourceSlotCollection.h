#pragma once
#include <cstdint>
#include <numeric>
#include <stack>
#include <vector>
namespace photon
{
	//struct ResourceHandle
	//{
	//	uint64_t index = 0;
	//	uint32_t generation = 0;
	//	// Handle = 0 表示非有效handle
	//	bool IsValid() const { return generation != 0; }
	//	auto operator<=>(const ResourceHandle&) const = default;
	//};

	template<typename HandleT, typename ResourceT>
	class ResourceSlotCollection
	{
	protected:
		struct Entry
		{
			bool occupied = false;
			uint32_t generation = 0;
			ResourceT resource;
		};

	public:

		HandleT AllocateHandle()
		{
			if (!m_frees.empty())
			{
				uint32_t freeidx = m_frees.top();
				m_frees.pop();

				m_entries[freeidx].occupied = true;
				++m_entries[freeidx].generation;

				HandleT h = {};
				h.handle.index = freeidx;
				h.handle.generation = m_entries[freeidx].generation;
				return h;
			}

			Entry e;
			e.occupied = true;
			e.generation = 1;
			m_entries.push_back(std::move(e));

			HandleT h = {};
			h.handle.index = static_cast<uint64_t>(m_entries.size() - 1);
			h.handle.generation = 1;
			return h;
		}

		Entry* TryGetEntry(HandleT h)
		{
			if (!IsHandleValid(h))
				return nullptr;

			Entry& e = m_entries[static_cast<uint32_t>(h.handle.index)];
			return &e;
		}

		const Entry* TryGetEntry(HandleT h) const
		{
			if (!IsHandleValid(h))
				return nullptr;

			const Entry& e = m_entries[static_cast<uint32_t>(h.handle.index)];
			return &e;
		}

		void FreeHandle(HandleT h)
		{
			Entry* e = TryGetEntry(h);

			if (!e)
				return;
			e->occupied = false;
			e->resource = {};

			m_frees.push(h.handle.index);
		}

		bool IsHandleValid(HandleT h) const
		{
			if (!h.handle.IsValid() || h.handle.index >= m_entries.size())
				return false;
			const Entry& e = m_entries[static_cast<uint32_t>(h.handle.index)];
			if (!e.occupied || e.generation != h.handle.generation)
				return false;

			return true;
		}

		void ResetAll()
		{
			m_entries.clear();
			m_entries.resize(0);
			while (!m_frees.empty())
				m_frees.pop();
		}

		std::vector<Entry>& GetEntries()
		{
			return m_entries;
		}

	private:
		std::vector<Entry> m_entries;
		std::stack<uint32_t> m_frees;
	};
}