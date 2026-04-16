#pragma once

#include <cstdint>

namespace photon
{
	enum class MaterialDomain : uint8_t
	{
		Surface,
		UI,
		PostProcess,
		Sky,
		Shadow,
	};

	enum class MaterialBlendMode : uint8_t
	{
		Opaque,
		Masked,
		Translucent,
		Additive,
		PremultipliedAlpha,
	};

	enum class MaterialShadingModel : uint8_t
	{
		Unlit,
		DefaultLit,
		Skybox,
	};

	enum class MaterialCullMode : uint8_t
	{
		Default,
		None,
		Front,
		Back,
	};

	enum class MaterialDepthMode : uint8_t
	{
		Default,
		ReadWrite,
		ReadOnly,
		Disabled,
		Skybox,
	};

	struct MaterialRenderStateDesc
	{
		MaterialDomain domain = MaterialDomain::Surface;
		MaterialBlendMode blendMode = MaterialBlendMode::Opaque;
		MaterialShadingModel shadingModel = MaterialShadingModel::DefaultLit;
		MaterialCullMode cullMode = MaterialCullMode::Default;
		MaterialDepthMode depthMode = MaterialDepthMode::Default;

		bool twoSided = false;
		bool wireframe = false;
		bool castShadow = true;
		bool alphaClip = false;
		float alphaCutoff = 0.5f;

		void Canonicalize()
		{
			if (blendMode != MaterialBlendMode::Masked)
			{
				alphaClip = false;
				alphaCutoff = 0.5f;
			}

			if (domain == MaterialDomain::UI || domain == MaterialDomain::PostProcess)
			{
				castShadow = false;
			}

			if (twoSided && cullMode == MaterialCullMode::Default)
			{
				cullMode = MaterialCullMode::None;
			}

			if (domain == MaterialDomain::Sky)
			{
				shadingModel = MaterialShadingModel::Skybox;
			}
		}
	};
}
