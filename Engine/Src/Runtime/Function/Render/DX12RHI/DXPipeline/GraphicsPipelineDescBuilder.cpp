#include "GraphicsPipelineDescBuilder.h"

namespace photon
{
	void PassPipelineStateOverrides::Canonicalize()
	{
		rtLayout.Canonicalize();
		sampleDesc.Canonicalize();
		if (hasRasterOverride)
			rasterOverride.Canonicalize();
		if (hasDepthStencilOverride)
			depthStencilOverride.Canonicalize();
		if (hasBlendOverride)
			blendOverride.Canonicalize();
	}

	namespace
	{
		CullMode ResolveCullMode(const MaterialRenderStateDesc& materialState)
		{
			switch (materialState.cullMode)
			{
			case MaterialCullMode::None: return CullMode::None;
			case MaterialCullMode::Front: return CullMode::Front;
			case MaterialCullMode::Back: return CullMode::Back;
			case MaterialCullMode::Default:
			default:
				return materialState.twoSided ? CullMode::None : CullMode::Back;
			}
		}
	}

	namespace PassPipelinePresets
	{
		PassPipelineStateOverrides ForwardColor(TextureFormat colorFormat, TextureFormat depthFormat)
		{
			PassPipelineStateOverrides d = {};
			d.rtLayout.rtvCount = 1;
			d.rtLayout.rtvFormats[0] = colorFormat;
			d.rtLayout.dsvFormat = depthFormat;
			return d;
		}

		PassPipelineStateOverrides Skybox(TextureFormat colorFormat, TextureFormat depthFormat)
		{
			PassPipelineStateOverrides d = ForwardColor(colorFormat, depthFormat);
			d.hasDepthStencilOverride = true;
			d.depthStencilOverride = PipelineStatePresets::SkyboxDepth();
			return d;
		}

		PassPipelineStateOverrides ShadowDepth(TextureFormat depthFormat,
			int32_t depthBias, float slopeScaledDepthBias, float depthBiasClamp)
		{
			PassPipelineStateOverrides d = {};
			d.rtLayout.rtvCount = 0;
			d.rtLayout.dsvFormat = depthFormat;
			d.hasRasterOverride = true;
			d.rasterOverride = PipelineStatePresets::ShadowCasterRaster(depthBias, slopeScaledDepthBias, depthBiasClamp);
			d.hasDepthStencilOverride = true;
			d.depthStencilOverride = PipelineStatePresets::DefaultDepth();
			d.hasBlendOverride = true;
			d.blendOverride = PipelineStatePresets::OpaqueBlend();
			return d;
		}

		PassPipelineStateOverrides UI(TextureFormat colorFormat)
		{
			PassPipelineStateOverrides d = {};
			d.rtLayout.rtvCount = 1;
			d.rtLayout.rtvFormats[0] = colorFormat;
			d.hasDepthStencilOverride = true;
			d.depthStencilOverride = PipelineStatePresets::NoDepth();
			d.hasBlendOverride = true;
			d.blendOverride = PipelineStatePresets::AlphaBlend();
			return d;
		}
	}

	RasterStateDesc BuildRasterStateFromMaterialState(const MaterialRenderStateDesc& inMaterialState)
	{
		MaterialRenderStateDesc materialState = inMaterialState;
		materialState.Canonicalize();

		RasterStateDesc desc = PipelineStatePresets::DefaultRaster();
		desc.fillMode = materialState.wireframe ? FillMode::Wireframe : FillMode::Solid;
		desc.cullMode = ResolveCullMode(materialState);
		return desc;
	}

	DepthStencilStateDesc BuildDepthStencilStateFromMaterialState(const MaterialRenderStateDesc& inMaterialState)
	{
		MaterialRenderStateDesc materialState = inMaterialState;
		materialState.Canonicalize();

		DepthStencilStateDesc desc = {};
		switch (materialState.depthMode)
		{
		case MaterialDepthMode::ReadWrite:
			desc = PipelineStatePresets::DefaultDepth();
			break;
		case MaterialDepthMode::ReadOnly:
			desc = PipelineStatePresets::ReadOnlyDepth();
			break;
		case MaterialDepthMode::Disabled:
			desc = PipelineStatePresets::NoDepth();
			break;
		case MaterialDepthMode::Skybox:
			desc = PipelineStatePresets::SkyboxDepth();
			break;
		case MaterialDepthMode::Default:
		default:
			if (materialState.domain == MaterialDomain::UI || materialState.domain == MaterialDomain::PostProcess)
			{
				desc = PipelineStatePresets::NoDepth();
			}
			else if (materialState.domain == MaterialDomain::Sky || materialState.shadingModel == MaterialShadingModel::Skybox)
			{
				desc = PipelineStatePresets::SkyboxDepth();
			}
			else if (materialState.blendMode == MaterialBlendMode::Translucent
				|| materialState.blendMode == MaterialBlendMode::Additive
				|| materialState.blendMode == MaterialBlendMode::PremultipliedAlpha)
			{
				desc = PipelineStatePresets::ReadOnlyDepth();
			}
			else
			{
				desc = PipelineStatePresets::DefaultDepth();
			}
			break;
		}
		return desc;
	}

	BlendStateDesc BuildBlendStateFromMaterialState(const MaterialRenderStateDesc& inMaterialState)
	{
		MaterialRenderStateDesc materialState = inMaterialState;
		materialState.Canonicalize();

		switch (materialState.blendMode)
		{
		case MaterialBlendMode::Translucent:
			return PipelineStatePresets::AlphaBlend();
		case MaterialBlendMode::Additive:
			return PipelineStatePresets::AdditiveBlend();
		case MaterialBlendMode::PremultipliedAlpha:
			return PipelineStatePresets::PremultipliedAlphaBlend();
		case MaterialBlendMode::Opaque:
		case MaterialBlendMode::Masked:
		default:
			return PipelineStatePresets::OpaqueBlend();
		}
	}

	bool BuildGraphicsPipelineDescFromShader(
		const IResourceView& resources,
		ShaderHandle shaderHandle,
		const MaterialRenderStateDesc& inMaterialState,
		uint64_t vertexLayoutHash,
		const PassPipelineStateOverrides& inPassOverrides,
		GraphicsPipelineDesc& outDesc)
	{
		const ShaderProgramRenderResource* shaderRR = resources.GetShaderProgramRenderResource(shaderHandle);
		if (!shaderRR)
			return false;

		MaterialRenderStateDesc materialState = inMaterialState;
		materialState.Canonicalize();

		PassPipelineStateOverrides passOverrides = inPassOverrides;
		passOverrides.Canonicalize();

		GraphicsPipelineDesc desc = {};
		desc.shaderProgram = shaderHandle;
		desc.rootSignature = shaderRR->rootSignature;
		desc.vertexLayoutHash = vertexLayoutHash;
		desc.rtLayout = passOverrides.rtLayout;
		desc.topologyType = passOverrides.topologyType;
		desc.sampleDesc = passOverrides.sampleDesc;
		desc.sampleMask = passOverrides.sampleMask;
		desc.rasterState = passOverrides.hasRasterOverride ? passOverrides.rasterOverride : BuildRasterStateFromMaterialState(materialState);
		desc.depthStencilState = passOverrides.hasDepthStencilOverride ? passOverrides.depthStencilOverride : BuildDepthStencilStateFromMaterialState(materialState);
		desc.blendState = passOverrides.hasBlendOverride ? passOverrides.blendOverride : BuildBlendStateFromMaterialState(materialState);
		desc.Canonicalize();

		outDesc = desc;
		return true;
	}

	bool BuildGraphicsPipelineDescForMaterial(
		const IResourceView& resources,
		MaterialHandle materialHandle,
		uint64_t vertexLayoutHash,
		const PassPipelineStateOverrides& passOverrides,
		GraphicsPipelineDesc& outDesc)
	{
		const MaterialRenderResource* materialRR = resources.GetMaterialRenderResource(materialHandle);
		if (!materialRR)
			return false;

		const MaterialAsset* materialAsset = resources.GetMaterialAsset(materialRR->assetGuid);
		if (!materialAsset)
			return false;

		return BuildGraphicsPipelineDescFromShader(
			resources,
			materialRR->shaderProgram,
			materialAsset->renderState,
			vertexLayoutHash,
			passOverrides,
			outDesc);
	}

	PipelineStateHandle GetOrCreateGraphicsPipelineForMaterial(
		PipelineStateCache& cache,
		const IResourceView& resources,
		MaterialHandle materialHandle,
		uint64_t vertexLayoutHash,
		const PassPipelineStateOverrides& passOverrides)
	{
		GraphicsPipelineDesc desc = {};
		if (!BuildGraphicsPipelineDescForMaterial(resources, materialHandle, vertexLayoutHash, passOverrides, desc))
			return {};
		return cache.GetOrCreateGraphics(desc);
	}
}
