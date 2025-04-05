#include "ResourceLoader.h"
#include "Platform/FileSystem/FileSystem.h"
#include "Function/Util/RenderUtil.h"

namespace photon 
{



	std::pair<std::shared_ptr<Buffer>, std::shared_ptr<Texture2D>> ResourceLoader::LoadTexture(const std::filesystem::path& filepath, bool bForceLoadSRGB)
	{
		std::unique_ptr<uint8_t[]> decodedData;
		D3D12_SUBRESOURCE_DATA subresource;
		auto Tex = m_Rhi->LoadTextureFromFile(filepath, decodedData, subresource, 0, bForceLoadSRGB);
		Tex->name = filepath.generic_wstring();
		UINT64 uploadBufferSize = GetRequiredIntermediateSize(Tex->gpuResource.Get(), 0, 1);
		BufferDesc desc;
		desc.bufferSizeInBytes = uploadBufferSize;
		desc.heapProp = ResourceHeapProperties::Upload;
		auto Upload = m_Rhi->CreateBuffer(desc);
		m_Rhi->CopySubResourceDataCpuToGpu(Tex.get(), Upload.get(), 0, &subresource);
		m_Rhi->ResourceStateTransform(Tex.get(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
		return std::make_pair(Upload, Tex);
	}

	std::shared_ptr<photon::Model> ResourceLoader::PreLoadModel(const std::filesystem::path& filepath)
	{
		auto retModel = std::make_shared<Model>();
		retModel->name = std::filesystem::canonical(filepath);
		bool loadedOver = LoadAssimpSceneModel(filepath, retModel);
		retModel->SetLoadOver(loadedOver);
		return retModel;
	}


	bool ResourceLoader::LoadAssimpSceneModel(const std::filesystem::path& filepath, std::shared_ptr<photon::Model> model)
	{
		Assimp::Importer importer;
		auto wstr = filepath.generic_wstring();
		auto str = WString2String(wstr);
		const aiScene* scene = importer.ReadFile(str, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);
		if (scene == nullptr)
		{
			LOG_ERROR("Failed Load Model: {}", WString2String(filepath.generic_wstring()));
			return false;
		}
		ProcessSingleAssimpNode(scene->mRootNode, scene, model);

		return true;
	}

	void ResourceLoader::ProcessSingleAssimpNode(aiNode* node, const aiScene* scene, std::shared_ptr<photon::Model> model)
	{
		for (int i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			LoadAssimpMesh(mesh, scene, model);
		}

		for (int i = 0; i < node->mNumChildren; ++i)
		{
			ProcessSingleAssimpNode(node->mChildren[i], scene, model);
		}
	}

	void ResourceLoader::LoadAssimpMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<photon::Model> model)
	{
		auto meshInfo = std::make_shared<ModelMeshRelateInfo>();

		meshInfo->meshDesc.type = VertexType::VertexSimple;
		meshInfo->meshDesc.name = String2WString(mesh->mName.C_Str());
		meshInfo->meshDesc.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		meshInfo->name = meshInfo->meshDesc.name;

		assert(mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE);
		std::vector<VertexSimple> vertices;
		vertices.reserve(mesh->mNumVertices);
		std::vector<uint32_t> indices;
		indices.reserve(3 * mesh->mNumFaces);

		for (int i = 0; i < mesh->mNumVertices; ++i)
		{
			VertexSimple vert;
			vert.position = ((Vector3*)mesh->mVertices)[i];
			vert.normal = ((Vector3*)mesh->mNormals)[i];
			vert.tangent = ((Vector3*)mesh->mTangents)[i];
			assert(mesh->HasTextureCoords(0));
			vert.texCoord = Vector2{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			vertices.push_back(vert);
		}

		for (int i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace& face = mesh->mFaces[i];
			for (int j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		meshInfo->meshDesc.vertexRawData = RenderUtil::CreateD3DBlob(vertices.data(), vertices.size() * sizeof(VertexSimple));
		meshInfo->meshDesc.indexRawData = RenderUtil::CreateD3DBlob(indices.data(), indices.size() * sizeof(uint32_t));

		model->meshes.push_back(meshInfo);

		if(mesh->mMaterialIndex >= 0)
		{
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
			int diffuseMapCount = mat->GetTextureCount(aiTextureType_DIFFUSE);
			int normalMapCount = mat->GetTextureCount(aiTextureType_NORMALS);
			int roughnessMapCount = mat->GetTextureCount(aiTextureType_SPECULAR);
			LoadAssimpMaterialTextures(mat, aiTextureType_DIFFUSE, model);
			LoadAssimpMaterialTextures(mat, aiTextureType_NORMALS, model);
			LoadAssimpMaterialTextures(mat, aiTextureType_SPECULAR, model);
			if(diffuseMapCount == 0 || normalMapCount == 0 || roughnessMapCount == 0)
			{
				TryLoadModelTexturesFromFile(model);
			}
		}
		else 
		{
			TryLoadModelTexturesFromFile(model);
		}

	}

	void ResourceLoader::LoadAssimpMaterialTextures(aiMaterial* mat, aiTextureType type, std::shared_ptr<photon::Model> model)
	{
		for(int i = 0; i < mat->GetTextureCount(type); ++i)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			std::wstring path = String2WString(str.C_Str());
			model->AddAssimpTexturePathToBack(path, type);
		}
	}

	void ResourceLoader::TryLoadModelTexturesFromFile(std::shared_ptr<photon::Model> model)
	{
		auto backInfo = model->meshes.back();
		auto folder = Path::GetFilePureName(model->name);
		if(backInfo->diffuseTexturePaths.empty())
		{
			std::filesystem::path diffuseMapPathJpg = (folder + diffuseMapSuffix) + L".jpg";
			std::filesystem::path diffuseMapPathPng = (folder + diffuseMapSuffix) + L".png";
			if(std::filesystem::exists(diffuseMapPathJpg))
			{
				backInfo->diffuseTexturePaths.push_back(diffuseMapPathJpg);
			}
			else if(std::filesystem::exists(diffuseMapPathPng))
			{
				backInfo->diffuseTexturePaths.push_back(diffuseMapPathPng);
			}
		}
		if(backInfo->normalTexturePaths.empty())
		{
			std::filesystem::path normalMapPathJpg = (folder + normalMapSuffix) + L".jpg";
			std::filesystem::path normalMapPathPng = (folder + normalMapSuffix) + L".png";
			if (std::filesystem::exists(normalMapPathJpg))
			{
				backInfo->normalTexturePaths.push_back(normalMapPathJpg);
			}
			else if (std::filesystem::exists(normalMapPathPng))
			{
				backInfo->normalTexturePaths.push_back(normalMapPathPng);
			}
		}
		if(backInfo->roughnessTexturePaths.empty())
		{
			std::filesystem::path roughnessMapPathJpg = (folder + roughnessMapSuffix) + L".jpg";
			std::filesystem::path roughnessMapPathPng = (folder + roughnessMapSuffix) + L".png";
			std::filesystem::path SpecularMapPathJpg = (folder + specularMapSuffix) + L".jpg";
			std::filesystem::path SpecularMapPathPng = (folder + specularMapSuffix) + L".png";
			if (std::filesystem::exists(roughnessMapPathJpg))
			{
				backInfo->roughnessTexturePaths.push_back(roughnessMapPathJpg);
			}
			else if (std::filesystem::exists(roughnessMapPathPng))
			{
				backInfo->roughnessTexturePaths.push_back(roughnessMapPathPng);
			}
			else if (std::filesystem::exists(SpecularMapPathJpg))
			{
				backInfo->roughnessTexturePaths.push_back(SpecularMapPathJpg);
				backInfo->bInverseRoughness = true;
			}
			else if (std::filesystem::exists(SpecularMapPathPng))
			{
				backInfo->roughnessTexturePaths.push_back(SpecularMapPathPng);
				backInfo->bInverseRoughness = true;
			}
		}
	}

}