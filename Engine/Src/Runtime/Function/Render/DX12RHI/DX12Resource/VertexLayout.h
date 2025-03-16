#pragma once 
#include <d3d12.h>
#include <vector>
#include <string>
#include <span>

namespace photon 
{
	class VertexLayout
	{
	public:
		VertexLayout() = default;
		VertexLayout(const VertexLayout&) = default;
		VertexLayout(VertexLayout&& layout) noexcept
		{
			m_InputLayout = std::move(layout.m_InputLayout);
			m_Offset = layout.m_Offset;
		}

		VertexLayout& operator=(const VertexLayout& rhs)
		{
			m_InputLayout = rhs.m_InputLayout;
			m_Offset = rhs.m_Offset;
			return *this;
		}

		VertexLayout& operator=(VertexLayout&& rhs)
		{
			m_InputLayout = std::move(rhs.m_InputLayout);
			m_Offset = rhs.m_Offset;
			return *this;
		}



		void PushLayout(const char* name, int semanticIdx, DXGI_FORMAT format, int inputSlot = 0, D3D12_INPUT_CLASSIFICATION classify = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA)
		{
			D3D12_INPUT_ELEMENT_DESC desc;
			desc.SemanticName = name;
			desc.SemanticIndex = semanticIdx;
			desc.Format = format;
			desc.InputSlot = inputSlot;
			desc.AlignedByteOffset = m_Offset;
			desc.InputSlotClass = classify;
			desc.InstanceDataStepRate = classify == D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA ? 0 : 1;
			m_InputLayout.push_back(desc);

			m_Offset += GetFormatSize(format);
		}

		
		std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayouts()
		{
			return m_InputLayout;
		}
		
		uint32_t GetSizeInBytes() const
		{
			return m_Offset;
		}
		
		static uint32_t GetFormatSize(DXGI_FORMAT format) 
		{
			switch(format)
			{
			case DXGI_FORMAT_R32G32B32_FLOAT:
				return 12;
			case DXGI_FORMAT_R32G32_FLOAT:
				return 8;
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
				return 16;
			case DXGI_FORMAT_R32_FLOAT:
				return 4;
			default:
				throw;
			}
		}

		
	private:
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;
		int m_Offset = 0;
	};


}