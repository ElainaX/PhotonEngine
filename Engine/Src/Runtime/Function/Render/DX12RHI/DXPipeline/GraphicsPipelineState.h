#pragma once

#include "Function/Render/Shader/Shader.h"
#include "Function/Render/RHI.h"
#include "Function/Render/RenderTypes.h"
#include "Function/Render/ResourceHandle.h"

#include <array>
#include <cstdint>
#include <functional>

#include "Function/Render/RenderObject/ImporterSettings.h"

namespace photon
{

	struct RasterStateDesc
	{
		FillMode fillMode = FillMode::Solid;
		CullMode cullMode = CullMode::Back;
		bool frontCCW = false;
		bool depthClipEnable = true;
		bool multisampleEnable = false;
		bool antialiasedLineEnable = false;
		int32_t depthBias = 0;
		float depthBiasClamp = 0.0f;
		float slopeScaledDepthBias = 0.0f;
		bool conservativeRaster = false;
		uint32_t forcedSampleCount = 0;

		void Canonicalize();
	};

	struct StencilFaceDesc
	{
		StencilOp failOp = StencilOp::Keep;
		StencilOp depthFailOp = StencilOp::Keep;
		StencilOp passOp = StencilOp::Keep;
		CompareOp func = CompareOp::Always;
	};

	struct DepthStencilStateDesc
	{
		bool depthEnable = true;
		DepthWriteMask depthWriteMask = DepthWriteMask::All;
		CompareOp depthFunc = CompareOp::Less;
		bool stencilEnable = false;
		uint8_t stencilReadMask = 0xff;
		uint8_t stencilWriteMask = 0xff;
		StencilFaceDesc frontFace = {};
		StencilFaceDesc backFace = {};

		void Canonicalize();
	};

	struct BlendStateDesc
	{
		bool alphaToCoverageEnable = false;
		bool blendEnable = false;
		bool logicOpEnable = false;
		BlendFactor srcColor = BlendFactor::One;
		BlendFactor dstColor = BlendFactor::Zero;
		BlendOp colorOp = BlendOp::Add;
		BlendFactor srcAlpha = BlendFactor::One;
		BlendFactor dstAlpha = BlendFactor::Zero;
		BlendOp alphaOp = BlendOp::Add;
		LogicOp logicOp = LogicOp::Noop;
		uint8_t colorWriteMask = ColorWrite_All;

		void Canonicalize();
	};

	struct SampleDescKey
	{
		uint16_t count = 1;
		uint16_t quality = 0;

		void Canonicalize();
	};

	struct RenderTargetLayoutKey
	{
		std::array<TextureFormat, 8> rtvFormats = {};
		uint32_t rtvCount = 0;
		TextureFormat dsvFormat = TextureFormat::Unknown;

		void Canonicalize();
	};

	struct GraphicsPipelineDesc
	{
		ShaderHandle shaderProgram;
		RootSignatureHandle rootSignature;
		uint64_t vertexLayoutHash = 0;

		RasterStateDesc rasterState = {};
		DepthStencilStateDesc depthStencilState = {};
		BlendStateDesc blendState = {};

		RenderTargetLayoutKey rtLayout = {};
		PrimitiveTopologyType topologyType = PrimitiveTopologyType::Triangle;
		SampleDescKey sampleDesc = {};
		uint32_t sampleMask = 0xffffffffu;

		void Canonicalize();
	};

	struct GraphicsPipelineKey
	{
		ShaderHandle shaderProgram;
		RootSignatureHandle rootSignature;
		uint64_t vertexLayoutHash = 0;

		RasterStateHandle rasterState;
		DepthStencilStateHandle depthStencilState;
		BlendStateHandle blendState;

		RenderTargetLayoutKey rtLayout = {};
		PrimitiveTopologyType topologyType = PrimitiveTopologyType::Triangle;
		SampleDescKey sampleDesc = {};
		uint32_t sampleMask = 0xffffffffu;
	};

	bool operator==(const RasterStateDesc& a, const RasterStateDesc& b);
	bool operator==(const StencilFaceDesc& a, const StencilFaceDesc& b);
	bool operator==(const DepthStencilStateDesc& a, const DepthStencilStateDesc& b);
	bool operator==(const BlendStateDesc& a, const BlendStateDesc& b);
	bool operator==(const SampleDescKey& a, const SampleDescKey& b);
	bool operator==(const RenderTargetLayoutKey& a, const RenderTargetLayoutKey& b);
	bool operator==(const GraphicsPipelineKey& a, const GraphicsPipelineKey& b);

	struct RasterStateDescHasher
	{
		size_t operator()(const RasterStateDesc& desc) const noexcept;
	};

	struct DepthStencilStateDescHasher
	{
		size_t operator()(const DepthStencilStateDesc& desc) const noexcept;
	};

	struct BlendStateDescHasher
	{
		size_t operator()(const BlendStateDesc& desc) const noexcept;
	};

	struct GraphicsPipelineKeyHasher
	{
		size_t operator()(const GraphicsPipelineKey& key) const noexcept;
	};

	namespace PipelineStatePresets
	{
		RasterStateDesc DefaultRaster();
		RasterStateDesc WireframeRaster();
		RasterStateDesc TwoSidedRaster();
		RasterStateDesc ShadowCasterRaster(int32_t depthBias, float slopeScaledDepthBias, float depthBiasClamp = 0.0f);

		DepthStencilStateDesc DefaultDepth();
		DepthStencilStateDesc ReadOnlyDepth(CompareOp func = CompareOp::LessEqual);
		DepthStencilStateDesc NoDepth();
		DepthStencilStateDesc SkyboxDepth();

		BlendStateDesc OpaqueBlend();
		BlendStateDesc AlphaBlend();
		BlendStateDesc PremultipliedAlphaBlend();
		BlendStateDesc AdditiveBlend();
	}
}
