#pragma once
#include <cstdint>

namespace photon
{
	enum class MeshUsage : uint8_t
	{
		Static,
		Dynamic,
		Stream
	};

	struct MeshImportSettings
	{
		bool generateNormals = false;
		bool generateTangents = false;
		bool optimizeIndices = true;
		bool mergeSubmeshesByMaterial = false;
		bool keepCpuData = true;
	};

	struct DynamicMeshState
	{
		bool vertexDirty = false;
		bool indexDirty = false;

		uint32_t dirtyVertexBegin = 0;
		uint32_t dirtyVertexEnd = 0;

		uint32_t dirtyIndexBegin = 0;
		uint32_t dirtyIndexEnd = 0;

		bool topologyChanged = false;
	};


	enum class TextureDimension : uint8_t
	{
		Tex2D,
		TexCube,
		Tex2DArray,
		Tex3D
	};

	struct TextureImportSettings
	{
		bool sRGB = true;
		bool generateMipmaps = true;
		bool isNormalMap = false;
		bool allowCompression = true;
	};

	enum class TextureFormat : uint16_t
	{
		Unknown,
		RGBA8_UNorm,
		RGBA8_UNorm_sRGB,
		BGRA8_UNorm,
		R8_UNorm,
		RG16_Float,
		RGBA16_Float,
		D24S8,
		D32_Float
	};

}
