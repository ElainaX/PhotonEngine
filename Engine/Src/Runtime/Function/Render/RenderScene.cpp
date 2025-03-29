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

	photon::CommonRenderItem* RenderScene::AddCommonRenderItem(std::shared_ptr<Mesh> mesh, Material* mat, Shader* shader, RenderLayer renderLayer, StaticModelObjectConstants objConstants)
	{
		auto commonRenderItem = std::make_shared<CommonRenderItem>();
		commonRenderItem->frameResourceInfo.objConstantIdx = StaticModelObjectConstants::s_CurrObjectIndex++;
		commonRenderItem->frameResourceInfo.objectConstants = objConstants;
		commonRenderItem->material = mat;
		commonRenderItem->meshCollection = m_MeshCollection.get();
		commonRenderItem->primitiveType = mesh->topology;
		commonRenderItem->meshGuid = mesh->guid;
		commonRenderItem->shader = shader;
		commonRenderItem->renderLayer = renderLayer;
		commonRenderItem->numFrameDirty = g_FrameContextCount;
		if(!m_MeshCollection->IsMeshLoaded(mesh->guid))
		{
			m_MeshCollection->PushMesh(mesh);
		}
		m_RenderItems.push_back(commonRenderItem);
		return commonRenderItem.get();
	}

	photon::Light* RenderScene::AddDirectionalLight(Vector3 strength, Vector3 dir)
	{
		if (directionalLights.size() >= MaxDirLights)
			return nullptr;
		directionalLights.push_back(CreateDirectionalLight(strength, dir));
		return &directionalLights.back();
	}

	photon::Light* RenderScene::AddPointLight(Vector3 strength, Vector3 position, Vector3 dir, float falloffStart, float falloffEnd)
	{
		if (pointLights.size() >= MaxPointLights)
			return nullptr;
		pointLights.push_back(CreatePointLight(strength, position, dir, falloffStart, falloffEnd));
		return &pointLights.back();
	}

	photon::Light* RenderScene::AddSpotLight(Vector3 strength, Vector3 position, Vector3 dir, float falloffStart, float falloffEnd, float spotPower)
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

	std::vector<photon::CommonRenderItem*> RenderScene::GetCommonRenderItems(RHI* rhi)
	{
		if(rhi)
		{
			m_MeshCollection->EndPush(rhi);
		}

		std::vector<photon::CommonRenderItem*> ret(m_RenderItems.size());
		for(int i = 0; i < ret.size(); ++i)
		{
			ret[i] = m_RenderItems[i].get();
		}
		return ret;
	}


}