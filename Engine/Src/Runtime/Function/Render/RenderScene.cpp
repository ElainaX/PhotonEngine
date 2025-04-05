#include "RenderScene.h"

#include <algorithm>

namespace photon 
{



	void RenderScene::Initialize(std::shared_ptr<RenderCamera> mainCamera)
	{
		m_MeshCollection = std::make_shared<RenderMeshCollection>();
		m_Cameras.push_back(mainCamera);
		directionalLights.reserve(MaxDirLights);
		pointLights.reserve(MaxPointLights);
		spotLights.reserve(MaxSpotLights);
	}

	photon::CommonRenderItem* RenderScene::AddCommonRenderItem(std::shared_ptr<Mesh> mesh, Material* mat, Shader* shader, RenderLayer renderLayer, StaticFrameResourceEditor frameResourceEditor)
	{
		auto commonRenderItem = CreateCommonRenderItem(mesh, mat, shader, renderLayer, frameResourceEditor);
		m_RenderItems.push_back(commonRenderItem);

		frameResourceEditor.bDirty = false;
		m_StaticFrameResourceEditors[commonRenderItem->GameObjectId] = frameResourceEditor;

		return commonRenderItem.get();
	}



	void RenderScene::AddModel(std::shared_ptr<Model> model, Shader* shader, RenderLayer renderLayer, StaticFrameResourceEditor frameResourceEditor)
	{
		std::pair<std::shared_ptr<Model>, std::vector<std::shared_ptr<CommonRenderItem>>> keyValue = std::make_pair(model, std::vector<std::shared_ptr<CommonRenderItem>>{});
		for(auto& meshInfo : model->meshes)
		{
			auto ritem = CreateCommonRenderItem(meshInfo->mesh, meshInfo->mat, shader, renderLayer, frameResourceEditor);
			keyValue.second.push_back(ritem);
			m_StaticFrameResourceEditors.insert({ ritem->GameObjectId, frameResourceEditor });
		}
		StaticFrameResourceEditor modelEditor;
		modelEditor.scale = { 0.1f, 0.1f, 0.1f };
		modelEditor.bDirty = true;
		m_StaticFrameResourceEditors.insert({ model->GameObjectId, modelEditor });
		m_ModelRenderItems.insert(keyValue);
	}

	photon::DirLight* RenderScene::AddDirectionalLight(Vector3 strength, Vector3 dir)
	{
		if (directionalLights.size() >= MaxDirLights)
			return nullptr;
		directionalLights.push_back(CreateDirectionalLight(strength, dir.normalisedCopy()));
		directionalLights.back().data.position = -dir;
		return &directionalLights.back();
	}

	photon::PointLight* RenderScene::AddPointLight(Vector3 strength, Vector3 position, float falloffStart, float falloffEnd)
	{
		if (pointLights.size() >= MaxPointLights)
			return nullptr;
		pointLights.push_back(CreatePointLight(strength, position, falloffStart, falloffEnd));
		return &pointLights.back();
	}

	photon::SpotLight* RenderScene::AddSpotLight(Vector3 strength, Vector3 position, Vector3 dir, float falloffStart, float falloffEnd, float spotPower)
	{
		if (spotLights.size() >= MaxSpotLights)
			return nullptr;
		spotLights.push_back(CreateSpotLight(strength, position, dir, falloffStart, falloffEnd, spotPower));
		return &spotLights.back();
	}

	void RenderScene::SetMainRenderCamera(RenderCamera* mainCam)
	{
		auto find_it = std::find_if(m_Cameras.begin(), m_Cameras.end(), [mainCam](auto sptr) {
				return sptr.get() == mainCam;
			});

		if(find_it != m_Cameras.end())
		{
			find_it->swap(*(m_Cameras.begin()));
		}
	}

	photon::RenderCamera* RenderScene::AddRenderCamera(std::shared_ptr<RenderCamera> camera, bool bSetMainCamera)
	{
		m_Cameras.push_back(camera);
		if(bSetMainCamera)
		{
			SetMainRenderCamera(camera.get());
		}
		return m_Cameras[0].get();
	}

	void RenderScene::RemoveRenderCamera(RenderCamera* camera)
	{
		auto find_it = std::find_if(m_Cameras.begin(), m_Cameras.end(), [camera](auto sptr) {
			return sptr.get() == camera;
			});
		m_Cameras.erase(find_it);
	}

	photon::RenderCamera* RenderScene::GetMainCamera()
	{
		if (m_Cameras.empty())
			return nullptr;
		return m_Cameras[0].get();
	}

	std::vector<photon::CommonRenderItem*> RenderScene::GetCommonRenderItems(RHI* rhi, bool bNeedModels)
	{
		if(rhi)
		{
			m_MeshCollection->EndPush(rhi);
		}

		std::vector<photon::CommonRenderItem*> ret(m_RenderItems.size());
		for(int i = 0; i < ret.size(); ++i)
		{
			auto frameResourceEditor = GetCommonRItemFrameResourceEditor(m_RenderItems[i]->GameObjectId);
			if(frameResourceEditor->bDirty)
			{
				m_RenderItems[i]->frameResourceInfo.objectConstants = frameResourceEditor->ToObjectConstants();
				m_RenderItems[i]->SetDirty();
				frameResourceEditor->bDirty = false;
			}
			ret[i] = m_RenderItems[i].get();
		}

		if(bNeedModels)
		{
			for (auto& keyval : m_ModelRenderItems)
			{
				auto model = keyval.first;
				auto ritems = keyval.second;

				auto frameEditor = GetCommonRItemFrameResourceEditor(model->GameObjectId);
				if (frameEditor->bDirty)
				{
					for (auto& ri : ritems)
					{
						ri->frameResourceInfo.objectConstants = frameEditor->ToObjectConstants();
						ri->SetDirty();
						frameEditor->bDirty = false;
					}
				}
				for (auto& ri : ritems)
				{
					ret.push_back(ri.get());
				}
			}
		}


		return ret;
	}


	photon::StaticFrameResourceEditor* RenderScene::GetCommonRItemFrameResourceEditor(uint64_t gameObjectId)
	{
		auto find_it = m_StaticFrameResourceEditors.find(gameObjectId);
		if(find_it != m_StaticFrameResourceEditors.end())
		{
			return &(find_it->second);
		}
		return nullptr;
	}

	std::unordered_map<std::shared_ptr<photon::Model>, std::vector<std::shared_ptr<photon::CommonRenderItem>>>& RenderScene::GetModelRenderItems()
	{
		return m_ModelRenderItems;
	}

	std::shared_ptr<photon::CommonRenderItem> RenderScene::CreateCommonRenderItem(std::shared_ptr<Mesh> mesh, Material* mat, Shader* shader, RenderLayer renderLayer, StaticFrameResourceEditor frameResourceEditor)
	{
		auto commonRenderItem = std::make_shared<CommonRenderItem>();
		commonRenderItem->frameResourceInfo.objConstantIdx = StaticModelObjectConstants::s_CurrObjectIndex++;
		commonRenderItem->frameResourceInfo.objectConstants = frameResourceEditor.ToObjectConstants();
		commonRenderItem->material = mat;
		commonRenderItem->meshCollection = m_MeshCollection.get();
		commonRenderItem->primitiveType = mesh->topology;
		commonRenderItem->meshGuid = mesh->guid;
		commonRenderItem->shader = shader;
		commonRenderItem->renderLayer = renderLayer;
		commonRenderItem->numFrameDirty = g_FrameContextCount;

		if (!m_MeshCollection->IsMeshLoaded(mesh->guid))
		{
			m_MeshCollection->PushMesh(mesh);
		}

		return commonRenderItem;
	}

}