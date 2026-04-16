#include "ResourceManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Function/Render/GeometryGenerator.h"
#include "Function/Render/DX12RHI/RenderResource/VertexType.h"
#include "Function/Util/RenderUtil.h"
#include "Platform/FileSystem/FileSystem.h"
#include "Function/Util/stb_image.h"

namespace photon
{
	namespace
	{
		Guid MakeGuidFromString(const std::string& s)
		{
			Guid g = {};
			g.guid = static_cast<uint64_t>(std::hash<std::string>{}(s));
			if (g.guid == 0)
				g.guid = 1;
			return g;
		}

		Guid MakeGuidFromPathAndSuffix(const std::filesystem::path& path, const std::string& suffix)
		{
			return MakeGuidFromString(path.generic_string() + "#" + suffix);
		}

		Guid MakeRuntimeTextureGuid(const std::string& debugName, uint64_t salt)
		{
			return MakeGuidFromString("__runtime_texture__#" + debugName + "#" + std::to_string(salt));
		}

		std::string ToLowerCopy(std::string s)
		{
			for (char& c : s)
			{
				c = static_cast<char>(::tolower(static_cast<unsigned char>(c)));
			}
			return s;
		}

		bool HasObjExtension(const std::filesystem::path& path)
		{
			return ToLowerCopy(path.extension().string()) == ".obj";
		}

		bool IsLikelyLinearTexture(const std::filesystem::path& path)
		{
			const std::string name = ToLowerCopy(path.stem().string());
			return name.find("normal") != std::string::npos
				|| name.find("rough") != std::string::npos
				|| name.find("metal") != std::string::npos
				|| name.find("orm") != std::string::npos
				|| name.find("ao") != std::string::npos
				|| name.find("occlusion") != std::string::npos
				|| name.find("mask") != std::string::npos
				|| name.find("height") != std::string::npos
				|| name.find("spec") != std::string::npos
				|| name.find("gloss") != std::string::npos;
		}

		TextureImportSettings BuildTextureImportSettingsByFilename(const std::filesystem::path& path)
		{
			TextureImportSettings s = {};
			s.sRGB = !IsLikelyLinearTexture(path);
			s.generateMipmaps = true;
			s.isNormalMap = ToLowerCopy(path.stem().string()).find("normal") != std::string::npos;
			s.allowCompression = true;
			return s;
		}

		DirectX::BoundingBox BuildBoundingBox(const Vector3& minP, const Vector3& maxP)
		{
			DirectX::BoundingBox box = {};
			box.Center = {
				(minP.x + maxP.x) * 0.5f,
				(minP.y + maxP.y) * 0.5f,
				(minP.z + maxP.z) * 0.5f
			};
			box.Extents = {
				(maxP.x - minP.x) * 0.5f,
				(maxP.y - minP.y) * 0.5f,
				(maxP.z - minP.z) * 0.5f
			};
			return box;
		}

		void ExpandBounds(Vector3& minP, Vector3& maxP, const Vector3& p)
		{
			minP.x = std::min(minP.x, p.x);
			minP.y = std::min(minP.y, p.y);
			minP.z = std::min(minP.z, p.z);

			maxP.x = std::max<float>(maxP.x, p.x);
			maxP.y = std::max<float>(maxP.y, p.y);
			maxP.z = std::max<float>(maxP.z, p.z);
		}

		bool TryGetFirstTexturePath(const aiMaterial* material, aiTextureType type, std::filesystem::path& outPath)
		{
			if (!material || material->GetTextureCount(type) == 0)
				return false;

			aiString aiPath;
			if (material->GetTexture(type, 0, &aiPath) != AI_SUCCESS)
				return false;

			if (aiPath.length == 0)
				return false;

			outPath = std::filesystem::path(aiPath.C_Str());
			return true;
		}

		constexpr uint32_t kAssimpImportFlags =
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ImproveCacheLocality |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_PreTransformVertices |
			aiProcess_FlipUVs |
			aiProcess_SortByPType |
			aiProcess_ConvertToLeftHanded;


		// TODO 扩展成一个导入类，可以负责Skeleton资源等，暂时只考虑static类
		MeshAsset BuildMeshAssetFromAssimpScene(
			const aiScene* scene,
			const std::filesystem::path& path,
			const std::vector<Guid>* materialGuids)
		{
			PHOTON_ASSERT(!path.empty(), "必须要输入有效Model路径");
			PHOTON_ASSERT(scene, "必须要输入有效Scene指针");
			MeshAsset asset = {};
			asset.meta.guid = MakeGuidFromPathAndSuffix(path, "mesh");
			asset.meta.sourcePath = path;
			asset.meta.debugName = path.stem().string();


			Vector3 globalMin(
				std::numeric_limits<float>::max(),
				std::numeric_limits<float>::max(),
				std::numeric_limits<float>::max());

			Vector3 globalMax(
				-std::numeric_limits<float>::max(),
				-std::numeric_limits<float>::max(),
				-std::numeric_limits<float>::max());



			asset.usage = MeshUsage::Static;
			asset.type = MeshType::StaticMesh;

			if (!scene->HasMeshes())
				return asset;

			std::vector<VertexPNTUV> vertices;
			std::vector<uint32_t> indices;

			if (materialGuids)
			{
				asset.defaultMaterialGuids = *materialGuids;
			}

			for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
			{
				const aiMesh* srcMesh = scene->mMeshes[meshIndex];
				if (!srcMesh || srcMesh->mNumVertices == 0 || srcMesh->mNumFaces == 0)
					continue;

				const uint32_t baseVertex = static_cast<uint32_t>(vertices.size());
				const uint32_t baseIndex = static_cast<uint32_t>(indices.size());

				photon::Vector3 subMin(
					                std::numeric_limits<float>::max(),
				                std::numeric_limits<float>::max(),
					std::numeric_limits<float>::max());

				photon::Vector3 subMax(
					                -std::numeric_limits<float>::max(),
					-std::numeric_limits<float>::max(),
					-std::numeric_limits<float>::max());

				for (uint32_t v = 0; v < srcMesh->mNumVertices; ++v)
				{
					photon::VertexPNTUV out = {};

					const aiVector3D& p = srcMesh->mVertices[v];
					out.pos = { p.x, p.y, p.z };

					if (srcMesh->HasNormals())
					{
						const aiVector3D& n = srcMesh->mNormals[v];
						out.normal = { n.x, n.y, n.z };
					}

					if (srcMesh->HasTangentsAndBitangents())
					{
						const aiVector3D& t = srcMesh->mTangents[v];
						out.tangent = { t.x, t.y, t.z };
					}

					if (srcMesh->HasTextureCoords(0))
					{
						const aiVector3D& uv = srcMesh->mTextureCoords[0][v];
						out.uv0 = { uv.x, uv.y };
					}

					vertices.push_back(out);
					ExpandBounds(subMin, subMax, out.pos);
					ExpandBounds(globalMin, globalMax, out.pos);
				}

				SubmeshAssetDesc sub = {};
				sub.name = srcMesh->mName.length > 0
					? srcMesh->mName.C_Str()
					: ("Submesh_" + std::to_string(meshIndex));
				sub.vertexStart = baseVertex;
				sub.vertexCount = srcMesh->mNumVertices;
				sub.indexStart = baseIndex;
				sub.materialSlot = srcMesh->mMaterialIndex;

				for (uint32_t f = 0; f < srcMesh->mNumFaces; ++f)
				{
					const aiFace& face = srcMesh->mFaces[f];
					if (face.mNumIndices != 3)
						continue;

					indices.push_back(baseVertex + face.mIndices[0]);
					indices.push_back(baseVertex + face.mIndices[1]);
					indices.push_back(baseVertex + face.mIndices[2]);
					sub.indexCount += 3;
				}

				sub.localBounds = BuildBoundingBox(subMin, subMax);
				asset.submeshes.push_back(sub);
			}

			if (!vertices.empty() && !indices.empty())
			{
				asset.cpuData = RenderUtil::BuildMeshCpuData(vertices, indices);
				asset.localBounds = BuildBoundingBox(globalMin, globalMax);
			}

			return asset;
		}

		std::string DefaultEntryPointForStage(ShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage::VS: return "VSMain";
			case ShaderStage::PS: return "PSMain";
			case ShaderStage::CS: return "CSMain";
			case ShaderStage::GS: return "GSMain";
			case ShaderStage::HS: return "HSMain";
			case ShaderStage::DS: return "DSMain";
			case ShaderStage::AS: return "ASMain";
			case ShaderStage::MS: return "MSMain";
			default: return "Main";
			}
		}

		std::string DefaultTargetProfileForStage(ShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage::VS: return "vs_5_1";
			case ShaderStage::PS: return "ps_5_1";
			case ShaderStage::CS: return "cs_5_1";
			case ShaderStage::GS: return "gs_5_1";
			case ShaderStage::HS: return "hs_5_1";
			case ShaderStage::DS: return "ds_5_1";
			case ShaderStage::AS: return "as_6_5";
			case ShaderStage::MS: return "ms_6_5";
			default: return "vs_5_1";
			}
		}

		std::array<bool, static_cast<size_t>(ShaderStage::Count)> BuildProbeMask()
		{
			std::array<bool, static_cast<size_t>(ShaderStage::Count)> ret = {};
			ret[ToIndex(ShaderStage::VS)] = true;
			ret[ToIndex(ShaderStage::PS)] = true;
			ret[ToIndex(ShaderStage::CS)] = true;
			ret[ToIndex(ShaderStage::GS)] = true;
			ret[ToIndex(ShaderStage::HS)] = true;
			ret[ToIndex(ShaderStage::DS)] = true;
			ret[ToIndex(ShaderStage::AS)] = true;
			ret[ToIndex(ShaderStage::MS)] = true;
			return ret;
		}
	}

	ShaderProgramLoadDesc ShaderProgramLoadDesc::GraphicsDefault()
	{
		ShaderProgramLoadDesc d;
		d.enableStages[ToIndex(ShaderStage::VS)] = true;
		d.enableStages[ToIndex(ShaderStage::PS)] = true;
		for (uint32_t i = 0; i < ShaderStageCount(); ++i)
		{
			auto s = static_cast<ShaderStage>(i);
			d.entryPoints[i] = DefaultEntryPointForStage(s);
			d.targetProfiles[i] = DefaultTargetProfileForStage(s);
		}
		return d;
	}

	ShaderProgramLoadDesc ShaderProgramLoadDesc::ComputeDefault()
	{
		ShaderProgramLoadDesc d;
		d.enableStages[ToIndex(ShaderStage::CS)] = true;
		for (uint32_t i = 0; i < ShaderStageCount(); ++i)
		{
			auto s = static_cast<ShaderStage>(i);
			d.entryPoints[i] = DefaultEntryPointForStage(s);
			d.targetProfiles[i] = DefaultTargetProfileForStage(s);
		}
		return d;
	}

	void ResourceManager::Initialize(DX12RHI* rhi, GpuResourceManager* gpuResMgr, DescriptorSystem* descriptorSystem,
		FrameAllocatorSystem* frameAllocatorSystem, CommandContextManager* cmdCtxMgr)
	{
		m_rhi = rhi;
		m_gpuResMgr = gpuResMgr;
		m_descriptorSystem = descriptorSystem;
		m_frameAllocatorSystem = frameAllocatorSystem;
		m_cmdCtxMgr = cmdCtxMgr;

		m_renderResourceFactory = std::make_shared<RenderResourceFactory>();
		m_renderResourceFactory->Initialize(m_rhi, m_gpuResMgr, m_frameAllocatorSystem, m_cmdCtxMgr);

		m_vertexLayoutRegistry = std::make_shared<VertexLayoutRegistry>();
		m_constantBufferCollection = std::make_shared<ConstantBufferCollection>();
		m_meshCollection = std::make_shared<MeshCollection>();
		m_textureCollection = std::make_shared<TextureCollection>();
		m_materialCollection = std::make_shared<MaterialCollection>();
		m_shaderProgramCollection = std::make_shared<ShaderProgramCollection>();
		m_rootSignatureCache = std::make_shared<RootSignatureCache>();
		m_pipelineStateCache = std::make_shared<PipelineStateCache>();

		m_constantBufferCollection->Initialize(
			m_renderResourceFactory.get(),
			m_descriptorSystem,
			m_gpuResMgr);

		m_meshCollection->Initialize(
			m_renderResourceFactory.get(),
			m_gpuResMgr,
			m_vertexLayoutRegistry.get());

		m_textureCollection->Initialize(m_renderResourceFactory.get(), m_gpuResMgr);

		m_materialCollection->Initialize(
			m_renderResourceFactory.get(),
			m_constantBufferCollection.get(),
			this);


		m_rootSignatureCache->Initialize(m_rhi);
		m_shaderProgramCollection->Initialize(m_rootSignatureCache.get());
		m_pipelineStateCache->Initialize(m_rhi, m_rootSignatureCache.get(), this, m_vertexLayoutRegistry.get());

		BuildDefaultAssets();
	}

	void ResourceManager::Shutdown()
	{
		m_pipelineStateCache.reset();
		if (m_shaderProgramCollection) m_shaderProgramCollection->Shutdown();
		if (m_rootSignatureCache) m_rootSignatureCache->Shutdown();
		if (m_materialCollection) m_materialCollection->Shutdown();
		if (m_textureCollection) m_textureCollection->Shutdown();
		if (m_meshCollection) m_meshCollection->Shutdown();
		if (m_renderResourceFactory) m_renderResourceFactory->Shutdown();
		
		m_modelAssets.clear();
		m_shaderProgramAssets.clear();
		m_shaderStageAssets.clear();
		m_materialAssets.clear();
		m_textureAssets.clear();
		m_meshAssets.clear();
		m_shaderGuidToHandle.clear();
		m_materialGuidToHandle.clear();
		m_textureGuidToHandle.clear();
		m_meshGuidToHandle.clear();

		m_fallbackShader = {};
		m_fallbackMaterial = {};
		m_fallbackTexture = {};
	}

	std::shared_ptr<ModelAsset> ResourceManager::LoadModel(const std::filesystem::path& path)
	{
		if (path.empty())
		{
			LOG_ERROR("LoadModel path is empty");
			return nullptr;
		}

		//if (!HasObjExtension(path))
		//{
		//	LOG_ERROR("LoadModel currently only supports .obj : {}", path.string());
		//	return nullptr;
		//}

		const std::filesystem::path absPath = std::filesystem::absolute(path).lexically_normal();
		const Guid modelGuid = MakeGuidFromPathAndSuffix(absPath, "model");

		auto itModel = m_modelAssets.find(modelGuid);
		if (itModel != m_modelAssets.end())
			return itModel->second;

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(absPath.string(), kAssimpImportFlags);
		if (!scene || !scene->HasMeshes())
		{
			LOG_ERROR("Assimp failed to load model: {}, reason: {}", absPath.string(), importer.GetErrorString());
			return nullptr;
		}

		std::vector<Guid> materialGuids(scene->mNumMaterials);

		const std::filesystem::path modelFolder = absPath.parent_path();

		for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
		{
			const Guid matGuid = MakeGuidFromPathAndSuffix(absPath, "material_" + std::to_string(i));
			materialGuids[i] = matGuid;

			if (m_materialGuidToHandle.find(matGuid) != m_materialGuidToHandle.end())
				continue;

			MaterialAsset matAsset = ImportMaterialAsset({}, m_fallbackShader);
			matAsset.meta.guid = matGuid;
			matAsset.meta.sourcePath = absPath;
			matAsset.meta.debugName = absPath.stem().string() + "_mat_" + std::to_string(i);
			matAsset.matConstant.semantic = ShaderParameterScope::Material;
			matAsset.matConstant.name = "PBR";
			PbrMaterialData matData = {};
			matAsset.matConstant.bytes.resize(sizeof(matData));
			const aiMaterial* aiMat = scene->mMaterials[i];
			if (aiMat)
			{
				aiColor3D diffuse(1.0f, 1.0f, 1.0f);
				if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS)
				{
					matData.diffuseAlbedo = Vector4(diffuse.r, diffuse.g, diffuse.b, 1.0);
				}

				float opacity = 1.0f;
				if (aiMat->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS)
				{

				}

				aiColor3D emissive(0.0f, 0.0f, 0.0f);
				if (aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) == AI_SUCCESS)
				{
					
				}

				int twoSided = 0;
				if (aiMat->Get(AI_MATKEY_TWOSIDED, twoSided) == AI_SUCCESS)
				{
					matAsset.renderState.twoSided = (twoSided != 0);
					matAsset.renderState.cullMode = twoSided ? MaterialCullMode::None : MaterialCullMode::Back;
				}

				std::filesystem::path texRelPath;
				auto tryBindTexture = [&](aiTextureType type, const char* bindingName)
					{
						if (!TryGetFirstTexturePath(aiMat, type, texRelPath))
							return;

						std::filesystem::path texAbsPath = texRelPath.is_absolute()
							? texRelPath
							: (modelFolder / texRelPath).lexically_normal();

						TextureHandle th = LoadTexture(texAbsPath);
						if (!th.handle.IsValid())
							return;

						MaterialTextureBinding binding = {};
						binding.name = bindingName;
						binding.textureGuid = MakeGuidFromPathAndSuffix(texAbsPath, "texture");
						matAsset.textureBindings.push_back(binding);
					};

				// .obj / .mtl 先按最常见的三张图处理
				tryBindTexture(aiTextureType_DIFFUSE, "baseColorMap");
				tryBindTexture(aiTextureType_NORMALS, "normalMap");

				// 先把 specular 临时映射成 roughnessMap，
				// 只是为了把管线先打通，后面你再细分 metallic/roughness 工作流
				tryBindTexture(aiTextureType_SPECULAR, "roughnessMap");
			}
			memcpy(matAsset.matConstant.bytes.data(), &matData, sizeof(matData));
			m_materialAssets[matAsset.meta.guid] = matAsset;

			MaterialHandle mh = m_materialCollection->CreateFromAsset(matAsset);
			m_materialGuidToHandle[matAsset.meta.guid] = mh;
		}

		MeshAsset mergedMeshAsset = BuildMeshAssetFromAssimpScene(scene, absPath, &materialGuids);
		if (!mergedMeshAsset.HasCpuData())
		{
			LOG_ERROR("LoadModel imported scene but generated empty mesh: {}", absPath.string());
			return nullptr;
		}

		const Guid meshGuid = mergedMeshAsset.meta.guid;

		// 如果之前 LoadMesh(path) 已经创建过 GPU mesh，
		// 这里不要重建 GPU，只覆盖 MeshAsset，让 defaultMaterialGuids 生效即可。
		auto itMeshHandle = m_meshGuidToHandle.find(meshGuid);
		if (itMeshHandle == m_meshGuidToHandle.end())
		{
			m_meshAssets[meshGuid] = mergedMeshAsset;
			MeshHandle mh = m_meshCollection->CreateFromAsset(mergedMeshAsset);
			m_meshGuidToHandle[meshGuid] = mh;
		}
		else
		{
			m_meshAssets[meshGuid] = mergedMeshAsset;
		}

		auto model = std::make_shared<ModelAsset>();
		model->meta.guid = modelGuid;
		model->meta.sourcePath = absPath;
		model->meta.debugName = absPath.stem().string();
		model->meshGuid = meshGuid;
		model->localBounds = mergedMeshAsset.localBounds;

		m_modelAssets[modelGuid] = model;
		return model;
	}

	void ResourceManager::BuildDefaultAssets()
	{
		TextureAsset white = ImportTextureAsset({});
		m_textureAssets[white.meta.guid] = white;
		m_fallbackTexture = m_textureCollection->CreateFromAsset(white);
		m_textureGuidToHandle[white.meta.guid] = m_fallbackTexture;

		MaterialAsset mat = ImportMaterialAsset({}, {});
		m_materialAssets[mat.meta.guid] = mat;
		m_fallbackMaterial = m_materialCollection->CreateFromAsset(mat);
		m_materialGuidToHandle[mat.meta.guid] = m_fallbackMaterial;
	}


	MeshHandle ResourceManager::LoadMesh(const std::filesystem::path& path)
	{
		if (path.empty())
		{
			Guid guid = MakeGuidFromPathAndSuffix(std::filesystem::path("__default_mesh__"), "mesh");
			auto it = m_meshGuidToHandle.find(guid);
			if (it != m_meshGuidToHandle.end())
				return it->second;

			MeshAsset asset = ImportStaticMeshAsset({});
			if (!asset.HasCpuData())
				return {};

			m_meshAssets[asset.meta.guid] = asset;
			MeshHandle h = m_meshCollection->CreateFromAsset(asset);
			m_meshGuidToHandle[asset.meta.guid] = h;
			return h;
		}

		if (!HasObjExtension(path))
		{
			LOG_ERROR("LoadMesh currently only supports .obj : {}", path.string());
			return {};
		}

		const std::filesystem::path absPath = std::filesystem::absolute(path).lexically_normal();
		Guid guid = MakeGuidFromPathAndSuffix(absPath, "mesh");

		auto it = m_meshGuidToHandle.find(guid);
		if (it != m_meshGuidToHandle.end())
			return it->second;

		MeshAsset asset = ImportStaticMeshAsset(absPath);
		if (!asset.HasCpuData())
		{
			LOG_ERROR("LoadMesh failed: {}", absPath.string());
			return {};
		}

		m_meshAssets[asset.meta.guid] = asset;
		MeshHandle h = m_meshCollection->CreateFromAsset(asset);
		m_meshGuidToHandle[asset.meta.guid] = h;
		return h;
	}

	TextureHandle ResourceManager::LoadTexture(const std::filesystem::path& path)
	{
		Guid guid = MakeGuidFromPathAndSuffix(path.empty() ? std::filesystem::path("__default_texture__") : path, "texture");
		auto it = m_textureGuidToHandle.find(guid);
		if (it != m_textureGuidToHandle.end())
			return it->second;

		TextureAsset asset = ImportTextureAsset(path);
		m_textureAssets[asset.meta.guid] = asset;
		TextureHandle h = m_textureCollection->CreateFromAsset(asset);
		m_textureGuidToHandle[asset.meta.guid] = h;
		if (!m_fallbackTexture.handle.IsValid())
			m_fallbackTexture = h;
		return h;
	}

	MaterialHandle ResourceManager::LoadMaterial(const std::filesystem::path& path)
	{
		Guid guid = MakeGuidFromPathAndSuffix(path.empty() ? std::filesystem::path("__default_material__") : path, "material");
		auto it = m_materialGuidToHandle.find(guid);
		if (it != m_materialGuidToHandle.end())
			return it->second;

		MaterialAsset asset = ImportMaterialAsset(path, m_fallbackShader);
		m_materialAssets[asset.meta.guid] = asset;
		MaterialHandle h = m_materialCollection->CreateFromAsset(asset);
		m_materialGuidToHandle[asset.meta.guid] = h;
		if (!m_fallbackMaterial.handle.IsValid())
			m_fallbackMaterial = h;
		return h;
	}

	MaterialHandle ResourceManager::CreatePBRMaterial(ShaderHandle shaderHandle, const std::string& debugName)
	{
		MaterialAsset asset = ImportMaterialAsset({}, shaderHandle);
		asset.meta.guid = MakeGuidFromString(debugName + std::to_string(m_materialAssets.size() + 1));
		asset.meta.debugName = debugName;
		m_materialAssets[asset.meta.guid] = asset;
		MaterialHandle h = m_materialCollection->CreateFromAsset(asset);
		m_materialGuidToHandle[asset.meta.guid] = h;
		return h;
	}

	ShaderHandle ResourceManager::LoadShaderProgram(const std::filesystem::path& path)
	{
		ShaderProgramLoadDesc probe = ShaderProgramLoadDesc::GraphicsDefault();
		probe.enableStages = BuildProbeMask();
		return LoadShaderProgram(path, probe);
	}

	ShaderHandle ResourceManager::LoadShaderProgram(const std::filesystem::path& path, const ShaderProgramLoadDesc& loadDesc)
	{
		Guid guid = MakeGuidFromPathAndSuffix(path, "shader_program");
		auto it = m_shaderGuidToHandle.find(guid);
		if (it != m_shaderGuidToHandle.end())
			return it->second;

		ShaderProgramAsset asset = ImportShaderProgramAsset(path, &loadDesc);
		std::array<const ShaderStageAsset*, static_cast<size_t>(ShaderStage::Count)> stageRefs = {};
		for (uint32_t i = 0; i < ShaderStageCount(); ++i)
		{
			ShaderStage stage = static_cast<ShaderStage>(i);
			const ShaderStageReference* ref = asset.TryGetStage(stage);
			if (!ref)
				continue;
			stageRefs[i] = GetShaderStageAsset(ref->stageAssetGuid);
		}

		ShaderHandle h = m_shaderProgramCollection->CreateFromAsset(asset, stageRefs);
		if (!h.handle.IsValid())
			return {};

		m_shaderProgramAssets[asset.meta.guid] = asset;
		m_shaderGuidToHandle[asset.meta.guid] = h;
		if (!m_fallbackShader.handle.IsValid())
			m_fallbackShader = h;
		return h;
	}

	bool ResourceManager::SetMaterialTextureBinding(MaterialHandle mat, std::string_view name, TextureHandle tex)
	{
		if (!mat.handle.IsValid())
			return false;

		const MaterialRenderResource* matRR = m_materialCollection->TryGet(mat);
		if (!matRR)
			return false;

		auto it = m_materialAssets.find(matRR->assetGuid);
		if (it == m_materialAssets.end())
			return false;

		MaterialAsset& asset = it->second;

		const TextureRenderResource* texRR = GetTextureRenderResource(tex);
		if (!texRR)
			return false;

		Guid texGuid = texRR->assetGuid;

		for (auto& binding : asset.textureBindings)
		{
			if (binding.name == name)
			{
				binding.textureGuid = texGuid;
				return true;
			}
		}

		MaterialTextureBinding binding = {};
		binding.name = std::string(name);
		binding.textureGuid = texGuid;
		asset.textureBindings.push_back(std::move(binding));
		return true;
	}

	TextureHandle ResourceManager::CreateRuntimeTexture2D(
		const DXTexture2DDesc& desc,
		TextureDimension dimension,
		const std::string& debugName)
	{
		if (!m_gpuResMgr || !m_descriptorSystem || !m_textureCollection)
			return {};

		TextureHandle h = m_textureCollection->AllocateHandle();
		auto* entry = m_textureCollection->TryGetEntry(h);
		if (!entry)
			return {};

		TextureRenderResource rr = {};
		rr.handle = h;
		rr.assetGuid = MakeRuntimeTextureGuid(debugName, h.handle.index);
		rr.dimension = dimension;
		rr.format = desc.format;
		rr.width = desc.width;
		rr.height = desc.height;
		rr.depth = 1;
		rr.arraySize = 1;
		rr.mipCount = desc.maxMipLevels;

		rr.texture = m_gpuResMgr->CreateTexture2D(desc);
		if (!rr.texture.IsValid())
		{
			m_textureCollection->FreeHandle(h);
			return {};
		}

		DXResource* texRes = m_gpuResMgr->GetResource(rr.texture);
		if (!texRes)
		{
			m_gpuResMgr->DestoryResourceImmediate(rr.texture);
			m_textureCollection->FreeHandle(h);
			return {};
		}

		if ((desc.flag & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0)
		{
			ViewDesc rtvDesc = {};
			rtvDesc.type = ViewType::RTV;
			rtvDesc.dimension = ViewDimension::Texture2D;
			rtvDesc.format = desc.format;
			rr.rtv = m_descriptorSystem->CreateDescriptor(texRes, rtvDesc);
		}

		// 非 depth 纹理都给 SRV
		if ((desc.flag & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) == 0)
		{
			ViewDesc srvDesc = {};
			srvDesc.type = ViewType::SRV;
			srvDesc.dimension = ViewDimension::Texture2D;
			srvDesc.format = desc.format;
			srvDesc.mipLevels = desc.maxMipLevels;
			rr.srv = m_descriptorSystem->CreateDescriptor(texRes, srvDesc);
		}

		if ((desc.flag & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0)
		{
			ViewDesc dsvDesc = {};
			dsvDesc.type = ViewType::DSV;
			dsvDesc.dimension = ViewDimension::Texture2D;
			dsvDesc.format = desc.format;
			rr.dsv = m_descriptorSystem->CreateDescriptor(texRes, dsvDesc);
		}
		else
		{
			ViewDesc srvDesc = {};
			srvDesc.type = ViewType::SRV;
			srvDesc.dimension = ViewDimension::Texture2D;
			srvDesc.format = desc.format;
			srvDesc.mipLevels = desc.maxMipLevels;
			rr.srv = m_descriptorSystem->CreateDescriptor(texRes, srvDesc);
		}

		entry->resource = std::move(rr);
		return h;
	}

	TextureHandle ResourceManager::CreateRuntimeTexture2DArray(
		const DXTexture2DArrayDesc& desc,
		TextureDimension dimension,
		const std::string& debugName)
	{
		if (!m_gpuResMgr || !m_descriptorSystem || !m_textureCollection)
			return {};

		TextureHandle h = m_textureCollection->AllocateHandle();
		auto* entry = m_textureCollection->TryGetEntry(h);
		if (!entry)
			return {};

		TextureRenderResource rr = {};
		rr.handle = h;
		rr.assetGuid = MakeRuntimeTextureGuid(debugName, h.handle.index);
		rr.dimension = dimension;
		rr.format = desc.format;
		rr.width = desc.width;
		rr.height = desc.height;
		rr.depth = 1;
		rr.arraySize = desc.arraySize;
		rr.mipCount = desc.maxMipLevels;

		rr.texture = m_gpuResMgr->CreateTexture2DArray(desc);
		if (!rr.texture.IsValid())
		{
			m_textureCollection->FreeHandle(h);
			return {};
		}

		DXResource* texRes = m_gpuResMgr->GetResource(rr.texture);
		if (!texRes)
		{
			m_gpuResMgr->DestoryResourceImmediate(rr.texture);
			m_textureCollection->FreeHandle(h);
			return {};
		}

		ViewDesc srvDesc = {};
		srvDesc.type = ViewType::SRV;
		srvDesc.format = desc.format;
		srvDesc.mipLevels = desc.maxMipLevels;
		srvDesc.firstArraySlice = 0;
		srvDesc.arraySize = desc.arraySize;
		srvDesc.dimension = (dimension == TextureDimension::Tex2DArray)
			? ViewDimension::Texture2DArray
			: ViewDimension::TextureCube;

		rr.srv = m_descriptorSystem->CreateDescriptor(texRes, srvDesc);

		if ((desc.flag & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0)
		{
			ViewDesc rtvDesc = {};
			rtvDesc.type = ViewType::RTV;
			rtvDesc.dimension = ViewDimension::Texture2DArray;
			rtvDesc.format = desc.format;
			rtvDesc.firstArraySlice = 0;
			rtvDesc.arraySize = desc.arraySize;
			rr.rtv = m_descriptorSystem->CreateDescriptor(texRes, rtvDesc);
		}

		if ((desc.flag & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0)
		{
			ViewDesc dsvDesc = {};
			dsvDesc.type = ViewType::DSV;
			dsvDesc.dimension = ViewDimension::Texture2DArray;
			dsvDesc.format = desc.format;
			dsvDesc.firstArraySlice = 0;
			dsvDesc.arraySize = desc.arraySize;
			rr.dsv = m_descriptorSystem->CreateDescriptor(texRes, dsvDesc);
		}

		entry->resource = std::move(rr);
		return h;
	}

	TextureHandle ResourceManager::CreateRuntimeTextureCube(
		const DXTexture2DArrayDesc& desc,
		const std::string& debugName)
	{
		return CreateRuntimeTexture2DArray(desc, TextureDimension::TexCube, debugName);
	}

	bool ResourceManager::DestroyTexture(TextureHandle h, bool eraseAsset)
	{
		if (!h.handle.IsValid() || !m_textureCollection || !m_gpuResMgr)
			return false;

		if (h == m_fallbackTexture)
			return false;

		const TextureRenderResource* rr = m_textureCollection->TryGet(h);
		if (!rr)
			return false;

		const Guid assetGuid = rr->assetGuid;
		const GpuResourceHandle gpuTex = rr->texture;

		// 先释放 GPU resource。
		// 你的 GpuResourceManager::FreeResHandle() 会顺带清 persistent views。
		if (gpuTex.IsValid())
		{
			m_gpuResMgr->DestoryResourceImmediate(gpuTex);
		}

		// 再释放 handle slot
		m_textureCollection->Destroy(h);

		// 最后清 asset / guid 映射，避免后续 LoadTexture(path) 命中陈旧 handle
		if (eraseAsset && assetGuid.IsValid())
		{
			m_textureGuidToHandle.erase(assetGuid);
			m_textureAssets.erase(assetGuid);
		}

		return true;
	}

	bool ResourceManager::DestroyMesh(MeshHandle h, bool eraseAsset)
	{
		if (!h.handle.IsValid() || !m_meshCollection)
			return false;

		const MeshRenderResource* rr = m_meshCollection->TryGet(h);
		if (!rr)
			return false;

		const Guid assetGuid = rr->assetGuid;

		// MeshCollection::Destroy 内部已经会把 pooled / dedicated buffer 的占用回收掉
		m_meshCollection->Destroy(h);

		if (eraseAsset && assetGuid.IsValid())
		{
			m_meshGuidToHandle.erase(assetGuid);
			m_meshAssets.erase(assetGuid);
		}

		return true;
	}

	bool ResourceManager::DestroyMaterial(MaterialHandle h, bool eraseAsset)
	{
		if (!h.handle.IsValid() || !m_materialCollection)
			return false;

		if (h == m_fallbackMaterial)
			return false;

		const MaterialRenderResource* rr = m_materialCollection->TryGet(h);
		if (!rr)
			return false;

		const Guid assetGuid = rr->assetGuid;

		// MaterialCollection::Destroy 内部已经会释放 PersistentConstantBufferHandle
		m_materialCollection->Destroy(h);

		if (eraseAsset && assetGuid.IsValid())
		{
			m_materialGuidToHandle.erase(assetGuid);
			m_materialAssets.erase(assetGuid);
		}

		return true;
	}

	bool ResourceManager::DestroyShaderProgram(ShaderHandle h, bool eraseAsset)
	{
		if (!h.handle.IsValid() || !m_shaderProgramCollection)
			return false;

		if (h == m_fallbackShader)
			return false;

		const ShaderProgramRenderResource* rr = m_shaderProgramCollection->TryGet(h);
		if (!rr)
			return false;

		const Guid assetGuid = rr->assetGuid;

		// 当前先只释放 shader collection entry
		// RootSignature / PSO 暂时不做精细级联回收，避免误伤共享 cache
		m_shaderProgramCollection->Destroy(h);

		if (eraseAsset && assetGuid.IsValid())
		{
			m_shaderGuidToHandle.erase(assetGuid);
			m_shaderProgramAssets.erase(assetGuid);
		}

		return true;
	}

	bool ResourceManager::UnloadModel(Guid modelGuid)
	{
		if (!modelGuid.IsValid())
			return false;

		auto it = m_modelAssets.find(modelGuid);
		if (it == m_modelAssets.end())
			return false;

		// 注意：这里只卸载 model asset 记录，
		// 不默认递归删除 mesh/material/texture，
		// 因为这些资源当前是按 guid/path 去重复用的，递归删很容易误删共享资源。
		m_modelAssets.erase(it);
		return true;
	}

	ShaderProgramAsset ResourceManager::ImportShaderProgramAsset(const std::filesystem::path& path, const ShaderProgramLoadDesc* loadDesc)
	{
		ShaderProgramAsset asset = {};
		asset.meta.guid = MakeGuidFromPathAndSuffix(path, "shader_program");
		asset.meta.sourcePath = path;
		asset.meta.debugName = path.stem().string();

		ShaderProgramLoadDesc desc = loadDesc ? *loadDesc : ShaderProgramLoadDesc::GraphicsDefault();
		asset.parameters = desc.parameters;
		asset.constantBuffers = desc.constantBuffers;

		for (uint32_t i = 0; i < ShaderStageCount(); ++i)
		{
			if (!desc.enableStages[i])
				continue;

			ShaderStage stage = static_cast<ShaderStage>(i);
			ShaderStageAsset stageAsset = {};
			stageAsset.meta.guid = MakeGuidFromPathAndSuffix(path, std::string("stage_") + ToString(stage));
			stageAsset.meta.sourcePath = path;
			stageAsset.meta.debugName = asset.meta.debugName + "_" + ToString(stage);
			stageAsset.stage = stage;
			stageAsset.sourcePath = path;
			stageAsset.entryPoint = desc.entryPoints[i].empty() ? DefaultEntryPointForStage(stage) : desc.entryPoints[i];
			stageAsset.targetProfile = desc.targetProfiles[i].empty() ? DefaultTargetProfileForStage(stage) : desc.targetProfiles[i];
			stageAsset.enableDebugInfo = desc.enableDebugInfo;
			stageAsset.treatWarningAsErrors = desc.treatWarningsAsErrors;

			m_shaderStageAssets[stageAsset.meta.guid] = stageAsset;
			asset.SetStage(stage, stageAsset.meta.guid);
		}

		return asset;
	}

	MeshAsset ResourceManager::ImportStaticMeshAsset(const std::filesystem::path& path)
	{
		// 空路径：保留你的默认 box 行为
		if (path.empty())
		{
			MeshAsset asset = {};
			asset.meta.guid = MakeGuidFromPathAndSuffix(std::filesystem::path("__default_mesh__"), "mesh");
			asset.meta.sourcePath = "";
			asset.meta.debugName = "DefaultBox";
			asset.usage = MeshUsage::Static;

			GeometryGenerator gen;
			auto box = gen.CreateBox(1.0f, 1.0f, 1.0f, 0);

			std::vector<VertexPNTUV> vertices;
			vertices.reserve(box.Vertices.size());

			Vector3 minP(
				std::numeric_limits<float>::max(),
				std::numeric_limits<float>::max(),
				std::numeric_limits<float>::max());

			Vector3 maxP(
				-std::numeric_limits<float>::max(),
				-std::numeric_limits<float>::max(),
				-std::numeric_limits<float>::max());

			for (const auto& v : box.Vertices)
			{
				VertexPNTUV out = {};
				out.pos = { v.Position.x, v.Position.y, v.Position.z };
				out.normal = { v.Normal.x, v.Normal.y, v.Normal.z };
				out.tangent = { v.TangentU.x, v.TangentU.y, v.TangentU.z };
				out.uv0 = { v.TexC.x, v.TexC.y };
				vertices.push_back(out);

				ExpandBounds(minP, maxP, out.pos);
			}

			asset.cpuData = RenderUtil::BuildMeshCpuData(vertices, box.Indices32);

			SubmeshAssetDesc sub = {};
			sub.name = asset.meta.debugName;
			sub.vertexStart = 0;
			sub.vertexCount = asset.cpuData.vertexCount;
			sub.indexStart = 0;
			sub.indexCount = asset.cpuData.indexCount;
			sub.materialSlot = 0;
			sub.localBounds = BuildBoundingBox(minP, maxP);
			asset.submeshes.push_back(sub);
			asset.localBounds = sub.localBounds;

			return asset;
		}

		if (!HasObjExtension(path))
		{
			LOG_ERROR("ImportMeshAsset currently only supports .obj : {}", path.string());
			return {};
		}

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path.string(), kAssimpImportFlags);
		if (!scene || !scene->HasMeshes())
		{
			LOG_ERROR("Assimp failed to load mesh: {}, reason: {}", path.string(), importer.GetErrorString());
			return {};
		}

		return BuildMeshAssetFromAssimpScene(scene, path, nullptr);
	}

	TextureAsset ResourceManager::ImportTextureAsset(const std::filesystem::path& path)
	{
		TextureAsset asset = {};

		if (path.empty())
		{
			asset.meta.guid = MakeGuidFromPathAndSuffix(std::filesystem::path("__default_texture__"), "texture");
			asset.meta.sourcePath = "";
			asset.meta.debugName = "WhiteTexture";
			asset.importSettings.sRGB = true;
			asset.cpuData.width = 1;
			asset.cpuData.height = 1;
			asset.cpuData.depth = 1;
			asset.cpuData.arraySize = 1;
			asset.cpuData.mipCount = 1;
			asset.cpuData.dimension = TextureDimension::Tex2D;
			asset.cpuData.format = TextureFormat::RGBA8_UNorm;
			asset.cpuData.pixels.resize(4);
			asset.cpuData.pixels[0] = std::byte{ 255 };
			asset.cpuData.pixels[1] = std::byte{ 255 };
			asset.cpuData.pixels[2] = std::byte{ 255 };
			asset.cpuData.pixels[3] = std::byte{ 255 };
			return asset;
		}

		asset.meta.guid = MakeGuidFromPathAndSuffix(path, "texture");
		asset.meta.sourcePath = path;
		asset.meta.debugName = path.stem().string();
		asset.importSettings = BuildTextureImportSettingsByFilename(path);

		int width = 0;
		int height = 0;
		int channels = 0;

		stbi_uc* pixels = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (!pixels || width <= 0 || height <= 0)
		{
			LOG_ERROR("stb_image failed to load texture: {}", path.string());
			if (pixels)
				stbi_image_free(pixels);
			return {};
		}

		asset.cpuData.width = static_cast<uint32_t>(width);
		asset.cpuData.height = static_cast<uint32_t>(height);
		asset.cpuData.depth = 1;
		asset.cpuData.arraySize = 1;
		asset.cpuData.mipCount = 1;
		asset.cpuData.dimension = TextureDimension::Tex2D;
		asset.cpuData.format = asset.importSettings.sRGB
			? TextureFormat::RGBA8_UNorm_sRGB
			: TextureFormat::RGBA8_UNorm;

		const size_t dataSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
		asset.cpuData.pixels.resize(dataSize);
		std::memcpy(asset.cpuData.pixels.data(), pixels, dataSize);

		stbi_image_free(pixels);
		return asset;
	}

	MaterialAsset ResourceManager::ImportMaterialAsset(const std::filesystem::path& path, ShaderHandle shaderHandle)
	{
		MaterialAsset asset = {};
		asset.meta.guid = MakeGuidFromPathAndSuffix(path.empty() ? std::filesystem::path("__default_material__") : path, "material");
		asset.meta.sourcePath = path;
		asset.meta.debugName = path.empty() ? "DefaultPBRMaterial" : path.stem().string();

		for (const auto& [guid, handle] : m_shaderGuidToHandle)
		{
			if (handle == shaderHandle)
			{
				asset.shaderProgramAssetGuid = guid;
				break;
			}
		}

		asset.renderState.domain = MaterialDomain::Surface;
		asset.renderState.blendMode = MaterialBlendMode::Opaque;
		asset.renderState.shadingModel = MaterialShadingModel::DefaultLit;
		asset.renderState.cullMode = MaterialCullMode::Back;
		asset.renderState.depthMode = MaterialDepthMode::ReadWrite;
		asset.renderState.twoSided = false;

		return asset;
	}

	const ShaderStageAsset* ResourceManager::GetShaderStageAsset(Guid guid) const
	{
		auto it = m_shaderStageAssets.find(guid);
		return it != m_shaderStageAssets.end() ? &it->second : nullptr;
	}

	const MeshAsset* ResourceManager::GetMeshAsset(Guid guid) const
	{
		auto it = m_meshAssets.find(guid);
		return it != m_meshAssets.end() ? &it->second : nullptr;
	}

	const MeshRenderResource* ResourceManager::GetMeshRenderResource(MeshHandle h) const
	{
		return m_meshCollection ? m_meshCollection->TryGet(h) : nullptr;
	}

	const TextureAsset* ResourceManager::GetTextureAsset(Guid guid) const
	{
		auto it = m_textureAssets.find(guid);
		return it != m_textureAssets.end() ? &it->second : nullptr;
	}

	const TextureRenderResource* ResourceManager::GetTextureRenderResource(TextureHandle h) const
	{
		return m_textureCollection ? m_textureCollection->TryGet(h) : nullptr;
	}

	const MaterialAsset* ResourceManager::GetMaterialAsset(Guid guid) const
	{
		auto it = m_materialAssets.find(guid);
		return it != m_materialAssets.end() ? &it->second : nullptr;
	}

	const MaterialRenderResource* ResourceManager::GetMaterialRenderResource(MaterialHandle h) const
	{
		return m_materialCollection ? m_materialCollection->TryGet(h) : nullptr;
	}

	const ShaderProgramAsset* ResourceManager::GetShaderProgramAsset(Guid guid) const
	{
		auto it = m_shaderProgramAssets.find(guid);
		return it != m_shaderProgramAssets.end() ? &it->second : nullptr;
	}

	const ShaderProgramRenderResource* ResourceManager::GetShaderProgramRenderResource(ShaderHandle h) const
	{
		return m_shaderProgramCollection ? m_shaderProgramCollection->TryGet(h) : nullptr;
	}

	bool ResourceManager::SetMeshShader(MeshHandle mesh, ShaderHandle shader)
	{
		if (!mesh.handle.IsValid() || !shader.handle.IsValid())
			return false;

		MeshRenderResource* meshRR = GetMeshRenderResourceMutable(mesh);
		MeshAsset* meshAsset = GetMeshAssetMutable(meshRR->assetGuid);
		ShaderProgramRenderResource* shaderRR = GetShaderProgramRenderResourceMutable(shader);
		if (!meshRR || !meshAsset || !shaderRR)
			return false;
		for (auto& matGuid : meshAsset->defaultMaterialGuids)
		{
			MaterialAsset* matAsset = GetMaterialAssetMutable(matGuid);
			MaterialRenderResource* matRR = GetMaterialRenderResourceMutable(GetMaterialHandleByGuid(matGuid));
			if (!matAsset || !matRR)
				return false;
			matAsset->shaderProgramAssetGuid = shaderRR->assetGuid;
			matRR->shaderProgram = shader;
		}
		return true;
	}

	MeshHandle ResourceManager::GetMeshHandleByGuid(Guid guid) const
	{
		auto it = m_meshGuidToHandle.find(guid);
		return it != m_meshGuidToHandle.end() ? it->second : MeshHandle{};
	}

	TextureHandle ResourceManager::GetTextureHandleByGuid(Guid guid) const
	{
		auto it = m_textureGuidToHandle.find(guid);
		return it != m_textureGuidToHandle.end() ? it->second : TextureHandle{};
	}

	MaterialHandle ResourceManager::GetMaterialHandleByGuid(Guid guid) const
	{
		auto it = m_materialGuidToHandle.find(guid);
		return it != m_materialGuidToHandle.end() ? it->second : MaterialHandle{};
	}

	ShaderHandle ResourceManager::GetShaderHandleByGuid(Guid guid) const
	{
		auto it = m_shaderGuidToHandle.find(guid);
		return it != m_shaderGuidToHandle.end() ? it->second : ShaderHandle{};
	}

	MaterialHandle ResourceManager::GetFallbackMaterial() const
	{
		return m_fallbackMaterial;
	}

	TextureHandle ResourceManager::GetFallbackTexture() const
	{
		return m_fallbackTexture;
	}

	ShaderHandle ResourceManager::GetFallbackShader() const
	{
		return m_fallbackShader;
	}

	ShaderStageAsset* ResourceManager::GetShaderStageAssetMutable(Guid guid)
	{
		auto it = m_shaderStageAssets.find(guid);
		return it != m_shaderStageAssets.end() ? &it->second : nullptr;
	}

	MeshAsset* ResourceManager::GetMeshAssetMutable(Guid guid)
	{
		auto it = m_meshAssets.find(guid);
		return it != m_meshAssets.end() ? &it->second : nullptr;
	}

	MeshRenderResource* ResourceManager::GetMeshRenderResourceMutable(MeshHandle h)
	{
		return m_meshCollection ? m_meshCollection->TryGet(h) : nullptr;
	}

	TextureAsset* ResourceManager::GetTextureAssetMutable(Guid guid)
	{
		auto it = m_textureAssets.find(guid);
		return it != m_textureAssets.end() ? &it->second : nullptr;
	}

	TextureRenderResource* ResourceManager::GetTextureRenderResourceMutable(TextureHandle h)
	{
		return m_textureCollection ? m_textureCollection->TryGet(h) : nullptr;
	}

	MaterialAsset* ResourceManager::GetMaterialAssetMutable(Guid guid)
	{
		auto it = m_materialAssets.find(guid);
		return it != m_materialAssets.end() ? &it->second : nullptr;
	}

	MaterialRenderResource* ResourceManager::GetMaterialRenderResourceMutable(MaterialHandle h)
	{
		return m_materialCollection ? m_materialCollection->TryGet(h) : nullptr;
	}

	ShaderProgramAsset* ResourceManager::GetShaderProgramAssetMutable(Guid guid)
	{
		auto it = m_shaderProgramAssets.find(guid);
		return it != m_shaderProgramAssets.end() ? &it->second : nullptr;
	}

	ShaderProgramRenderResource* ResourceManager::GetShaderProgramRenderResourceMutable(ShaderHandle h)
	{
		return m_shaderProgramCollection ? m_shaderProgramCollection->TryGet(h) : nullptr;
	}
}
