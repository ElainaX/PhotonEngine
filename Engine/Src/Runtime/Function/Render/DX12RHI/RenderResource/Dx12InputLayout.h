#pragma once 
#include <d3d12.h>
#include <vector>
#include <string>
#include <span>

#include "VertexType.h"
#include "Function/Util/RenderUtil.h"

namespace photon 
{
	class Dx12InputLayout
	{
	public:
		Dx12InputLayout() = default;

		explicit Dx12InputLayout(const VertexLayoutDesc& layout)
		{
			Build(layout);
		}

		void Build(const VertexLayoutDesc& layout)
		{
			m_elements.clear();
			m_elements.reserve(layout.elements.size());

			for (const auto& e : layout.elements)
			{
				D3D12_INPUT_ELEMENT_DESC desc = {};
				desc.SemanticName = ToSemanticName(e.semantic);
				desc.SemanticIndex = e.semanticIndex;
				desc.Format = RenderUtil::ToDxgiFormat(e.format);
				desc.InputSlot = e.inputSlot;
				desc.AlignedByteOffset = e.offset;
				desc.InputSlotClass = ToD3D12InputClassification(e.inputClass);
				desc.InstanceDataStepRate = e.instanceDataStepRate;

				m_elements.push_back(desc);
			}
		}

		D3D12_INPUT_LAYOUT_DESC GetDesc() const
		{
			D3D12_INPUT_LAYOUT_DESC desc = {};
			desc.pInputElementDescs = m_elements.empty() ? nullptr : m_elements.data();
			desc.NumElements = static_cast<UINT>(m_elements.size());
			return desc;
		}

		const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout() const
		{
			return m_elements;
		}
		
	private:
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_elements;
	};


}
