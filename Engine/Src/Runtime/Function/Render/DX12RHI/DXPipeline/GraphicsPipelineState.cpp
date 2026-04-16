#include "GraphicsPipelineState.h"

namespace photon
{
	void RasterStateDesc::Canonicalize()
	{
		if (fillMode != FillMode::Wireframe)
		{
			antialiasedLineEnable = false;
		}

		if (forcedSampleCount <= 1)
		{
			forcedSampleCount = 0;
		}
	}

	void DepthStencilStateDesc::Canonicalize()
	{
		if (!depthEnable)
		{
			depthWriteMask = DepthWriteMask::Zero;
			depthFunc = CompareOp::Always;
		}

		if (!stencilEnable)
		{
			stencilReadMask = 0xff;
			stencilWriteMask = 0xff;
			frontFace = {};
			backFace = {};
		}
	}

	void BlendStateDesc::Canonicalize()
	{
		if (logicOpEnable)
		{
			blendEnable = false;
			srcColor = BlendFactor::One;
			dstColor = BlendFactor::Zero;
			colorOp = BlendOp::Add;
			srcAlpha = BlendFactor::One;
			dstAlpha = BlendFactor::Zero;
			alphaOp = BlendOp::Add;
		}
		else if (!blendEnable)
		{
			srcColor = BlendFactor::One;
			dstColor = BlendFactor::Zero;
			colorOp = BlendOp::Add;
			srcAlpha = BlendFactor::One;
			dstAlpha = BlendFactor::Zero;
			alphaOp = BlendOp::Add;
			logicOp = LogicOp::Noop;
		}
	}

	void SampleDescKey::Canonicalize()
	{
		if (count == 0)
			count = 1;
		if (count <= 1)
			quality = 0;
	}

	void RenderTargetLayoutKey::Canonicalize()
	{
		if (rtvCount > rtvFormats.size())
			rtvCount = static_cast<uint32_t>(rtvFormats.size());
		for (uint32_t i = rtvCount; i < rtvFormats.size(); ++i)
			rtvFormats[i] = TextureFormat::Unknown;
	}

	void GraphicsPipelineDesc::Canonicalize()
	{
		rasterState.Canonicalize();
		depthStencilState.Canonicalize();
		blendState.Canonicalize();
		rtLayout.Canonicalize();
		sampleDesc.Canonicalize();
	}

	bool operator==(const RasterStateDesc& a, const RasterStateDesc& b)
	{
		return a.fillMode == b.fillMode &&
			a.cullMode == b.cullMode &&
			a.frontCCW == b.frontCCW &&
			a.depthClipEnable == b.depthClipEnable &&
			a.multisampleEnable == b.multisampleEnable &&
			a.antialiasedLineEnable == b.antialiasedLineEnable &&
			a.depthBias == b.depthBias &&
			a.depthBiasClamp == b.depthBiasClamp &&
			a.slopeScaledDepthBias == b.slopeScaledDepthBias &&
			a.conservativeRaster == b.conservativeRaster &&
			a.forcedSampleCount == b.forcedSampleCount;
	}

	bool operator==(const StencilFaceDesc& a, const StencilFaceDesc& b)
	{
		return a.failOp == b.failOp &&
			a.depthFailOp == b.depthFailOp &&
			a.passOp == b.passOp &&
			a.func == b.func;
	}

	bool operator==(const DepthStencilStateDesc& a, const DepthStencilStateDesc& b)
	{
		return a.depthEnable == b.depthEnable &&
			a.depthWriteMask == b.depthWriteMask &&
			a.depthFunc == b.depthFunc &&
			a.stencilEnable == b.stencilEnable &&
			a.stencilReadMask == b.stencilReadMask &&
			a.stencilWriteMask == b.stencilWriteMask &&
			a.frontFace == b.frontFace &&
			a.backFace == b.backFace;
	}

	bool operator==(const BlendStateDesc& a, const BlendStateDesc& b)
	{
		return a.alphaToCoverageEnable == b.alphaToCoverageEnable &&
			a.blendEnable == b.blendEnable &&
			a.logicOpEnable == b.logicOpEnable &&
			a.srcColor == b.srcColor &&
			a.dstColor == b.dstColor &&
			a.colorOp == b.colorOp &&
			a.srcAlpha == b.srcAlpha &&
			a.dstAlpha == b.dstAlpha &&
			a.alphaOp == b.alphaOp &&
			a.logicOp == b.logicOp &&
			a.colorWriteMask == b.colorWriteMask;
	}

	bool operator==(const SampleDescKey& a, const SampleDescKey& b)
	{
		return a.count == b.count && a.quality == b.quality;
	}

	bool operator==(const RenderTargetLayoutKey& a, const RenderTargetLayoutKey& b)
	{
		if (a.rtvCount != b.rtvCount || a.dsvFormat != b.dsvFormat)
			return false;
		for (uint32_t i = 0; i < a.rtvCount; ++i)
		{
			if (a.rtvFormats[i] != b.rtvFormats[i])
				return false;
		}
		return true;
	}

	bool operator==(const GraphicsPipelineKey& a, const GraphicsPipelineKey& b)
	{
		return a.shaderProgram == b.shaderProgram &&
			a.rootSignature == b.rootSignature &&
			a.vertexLayoutHash == b.vertexLayoutHash &&
			a.rasterState == b.rasterState &&
			a.depthStencilState == b.depthStencilState &&
			a.blendState == b.blendState &&
			a.rtLayout == b.rtLayout &&
			a.topologyType == b.topologyType &&
			a.sampleDesc == b.sampleDesc &&
			a.sampleMask == b.sampleMask;
	}

	static void HashFloat(size_t& seed, float value)
	{
		HashCombine(seed, std::hash<float>{}(value));
	}

	size_t RasterStateDescHasher::operator()(const RasterStateDesc& desc) const noexcept
	{
		size_t seed = 0;
		HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(desc.fillMode)));
		HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(desc.cullMode)));
		HashCombine(seed, std::hash<bool>{}(desc.frontCCW));
		HashCombine(seed, std::hash<bool>{}(desc.depthClipEnable));
		HashCombine(seed, std::hash<bool>{}(desc.multisampleEnable));
		HashCombine(seed, std::hash<bool>{}(desc.antialiasedLineEnable));
		HashCombine(seed, std::hash<int32_t>{}(desc.depthBias));
		HashFloat(seed, desc.depthBiasClamp);
		HashFloat(seed, desc.slopeScaledDepthBias);
		HashCombine(seed, std::hash<bool>{}(desc.conservativeRaster));
		HashCombine(seed, std::hash<uint32_t>{}(desc.forcedSampleCount));
		return seed;
	}

	size_t DepthStencilStateDescHasher::operator()(const DepthStencilStateDesc& desc) const noexcept
	{
		size_t seed = 0;
		HashCombine(seed, std::hash<bool>{}(desc.depthEnable));
		HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(desc.depthWriteMask)));
		HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(desc.depthFunc)));
		HashCombine(seed, std::hash<bool>{}(desc.stencilEnable));
		HashCombine(seed, std::hash<uint8_t>{}(desc.stencilReadMask));
		HashCombine(seed, std::hash<uint8_t>{}(desc.stencilWriteMask));
		auto hashFace = [&](const StencilFaceDesc& face)
			{
				HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(face.failOp)));
				HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(face.depthFailOp)));
				HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(face.passOp)));
				HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(face.func)));
			};
		hashFace(desc.frontFace);
		hashFace(desc.backFace);
		return seed;
	}

	size_t BlendStateDescHasher::operator()(const BlendStateDesc& desc) const noexcept
	{
		size_t seed = 0;
		HashCombine(seed, std::hash<bool>{}(desc.alphaToCoverageEnable));
		HashCombine(seed, std::hash<bool>{}(desc.blendEnable));
		HashCombine(seed, std::hash<bool>{}(desc.logicOpEnable));
		HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(desc.srcColor)));
		HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(desc.dstColor)));
		HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(desc.colorOp)));
		HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(desc.srcAlpha)));
		HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(desc.dstAlpha)));
		HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(desc.alphaOp)));
		HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(desc.logicOp)));
		HashCombine(seed, std::hash<uint8_t>{}(desc.colorWriteMask));
		return seed;
	}

	size_t GraphicsPipelineKeyHasher::operator()(const GraphicsPipelineKey& key) const noexcept
	{
		size_t seed = 0;
		HashCombine(seed, HashResourceHandle(key.shaderProgram.handle));
		HashCombine(seed, HashResourceHandle(key.rootSignature.handle));
		HashCombine(seed, std::hash<uint64_t>{}(key.vertexLayoutHash));
		HashCombine(seed, HashResourceHandle(key.rasterState.handle));
		HashCombine(seed, HashResourceHandle(key.depthStencilState.handle));
		HashCombine(seed, HashResourceHandle(key.blendState.handle));
		HashCombine(seed, std::hash<uint32_t>{}(key.rtLayout.rtvCount));
		for (uint32_t i = 0; i < key.rtLayout.rtvCount; ++i)
		{
			HashCombine(seed, std::hash<uint16_t>{}(static_cast<uint16_t>(key.rtLayout.rtvFormats[i])));
		}
		HashCombine(seed, std::hash<uint16_t>{}(static_cast<uint16_t>(key.rtLayout.dsvFormat)));
		HashCombine(seed, std::hash<uint8_t>{}(static_cast<uint8_t>(key.topologyType)));
		HashCombine(seed, std::hash<uint16_t>{}(key.sampleDesc.count));
		HashCombine(seed, std::hash<uint16_t>{}(key.sampleDesc.quality));
		HashCombine(seed, std::hash<uint32_t>{}(key.sampleMask));
		return seed;
	}

	namespace PipelineStatePresets
	{
		RasterStateDesc DefaultRaster()
		{
			return {};
		}

		RasterStateDesc WireframeRaster()
		{
			RasterStateDesc desc;
			desc.fillMode = FillMode::Wireframe;
			desc.cullMode = CullMode::None;
			return desc;
		}

		RasterStateDesc TwoSidedRaster()
		{
			RasterStateDesc desc;
			desc.cullMode = CullMode::None;
			return desc;
		}

		RasterStateDesc ShadowCasterRaster(int32_t depthBias, float slopeScaledDepthBias, float depthBiasClamp)
		{
			RasterStateDesc desc;
			desc.depthBias = depthBias;
			desc.slopeScaledDepthBias = slopeScaledDepthBias;
			desc.depthBiasClamp = depthBiasClamp;
			return desc;
		}

		DepthStencilStateDesc DefaultDepth()
		{
			return {};
		}

		DepthStencilStateDesc ReadOnlyDepth(CompareOp func)
		{
			DepthStencilStateDesc desc;
			desc.depthEnable = true;
			desc.depthWriteMask = DepthWriteMask::Zero;
			desc.depthFunc = func;
			return desc;
		}

		DepthStencilStateDesc NoDepth()
		{
			DepthStencilStateDesc desc;
			desc.depthEnable = false;
			desc.depthWriteMask = DepthWriteMask::Zero;
			desc.depthFunc = CompareOp::Always;
			return desc;
		}

		DepthStencilStateDesc SkyboxDepth()
		{
			DepthStencilStateDesc desc;
			desc.depthEnable = true;
			desc.depthWriteMask = DepthWriteMask::Zero;
			desc.depthFunc = CompareOp::LessEqual;
			return desc;
		}

		BlendStateDesc OpaqueBlend()
		{
			return {};
		}

		BlendStateDesc AlphaBlend()
		{
			BlendStateDesc desc;
			desc.blendEnable = true;
			desc.srcColor = BlendFactor::SrcAlpha;
			desc.dstColor = BlendFactor::InvSrcAlpha;
			desc.colorOp = BlendOp::Add;
			desc.srcAlpha = BlendFactor::One;
			desc.dstAlpha = BlendFactor::InvSrcAlpha;
			desc.alphaOp = BlendOp::Add;
			return desc;
		}

		BlendStateDesc PremultipliedAlphaBlend()
		{
			BlendStateDesc desc;
			desc.blendEnable = true;
			desc.srcColor = BlendFactor::One;
			desc.dstColor = BlendFactor::InvSrcAlpha;
			desc.colorOp = BlendOp::Add;
			desc.srcAlpha = BlendFactor::One;
			desc.dstAlpha = BlendFactor::InvSrcAlpha;
			desc.alphaOp = BlendOp::Add;
			return desc;
		}

		BlendStateDesc AdditiveBlend()
		{
			BlendStateDesc desc;
			desc.blendEnable = true;
			desc.srcColor = BlendFactor::One;
			desc.dstColor = BlendFactor::One;
			desc.colorOp = BlendOp::Add;
			desc.srcAlpha = BlendFactor::One;
			desc.dstAlpha = BlendFactor::One;
			desc.alphaOp = BlendOp::Add;
			return desc;
		}
	}
}
