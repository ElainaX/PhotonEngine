#include "RenderScene.h"
#include <algorithm>

namespace photon
{
	void RenderScene::Initialize(std::shared_ptr<RenderCamera> mainCamera)
	{
		m_cameras.clear();
		m_cameras.push_back(mainCamera);

		directionalLights.reserve(MaxDirLights);
		pointLights.reserve(MaxPointLights);
		spotLights.reserve(MaxSpotLights);
	}

	RenderItem* RenderScene::CreateRenderItem()
	{
		auto item = std::make_unique<RenderItem>();
		item->id = m_nextRenderItemId++;
		item->layers = RenderLayer::Opaque;
		item->flags.visible = true;
		item->flags.castShadow = true;
		item->flags.receiveShadow = true;
		item->flags.transparent = false;
		item->flags.doubleSided = false;
		item->flags.staticObject = true;

		auto* ret = item.get();
		m_renderItems.push_back(std::move(item));
		return ret;
	}

	std::vector<RenderItem*> RenderScene::GatherMutableRenderItems()
	{
		std::vector<RenderItem*> ret;
		ret.reserve(m_renderItems.size());
		for (auto& item : m_renderItems)
			ret.push_back(item.get());
		return ret;
	}

	std::vector<const RenderItem*> RenderScene::GatherRenderItems() const
	{
		std::vector<const RenderItem*> ret;
		ret.reserve(m_renderItems.size());
		for (const auto& item : m_renderItems)
			ret.push_back(item.get());
		return ret;
	}

	DirLight* RenderScene::AddDirectionalLight(Vector3 strength, Vector3 dir)
	{
		if (directionalLights.size() >= MaxDirLights)
			return nullptr;
		directionalLights.push_back(CreateDirectionalLight(strength, dir.normalisedCopy()));
		directionalLights.back().data.position = -dir;
		return &directionalLights.back();
	}

	PointLight* RenderScene::AddPointLight(Vector3 strength, Vector3 position, float falloffStart, float falloffEnd)
	{
		if (pointLights.size() >= MaxPointLights)
			return nullptr;
		pointLights.push_back(CreatePointLight(strength, position, falloffStart, falloffEnd));
		return &pointLights.back();
	}

	SpotLight* RenderScene::AddSpotLight(Vector3 strength, Vector3 position, Vector3 dir, float falloffStart, float falloffEnd, float spotPower)
	{
		if (spotLights.size() >= MaxSpotLights)
			return nullptr;
		spotLights.push_back(CreateSpotLight(strength, position, dir, falloffStart, falloffEnd, spotPower));
		return &spotLights.back();
	}

	void RenderScene::SetMainRenderCamera(RenderCamera* mainCam)
	{
		auto it = std::find_if(m_cameras.begin(), m_cameras.end(),
			[mainCam](const auto& p) { return p.get() == mainCam; });
		if (it != m_cameras.end())
			std::iter_swap(m_cameras.begin(), it);
	}

	RenderCamera* RenderScene::AddRenderCamera(std::shared_ptr<RenderCamera> camera, bool setMain)
	{
		m_cameras.push_back(camera);
		if (setMain)
			SetMainRenderCamera(camera.get());
		return camera.get();
	}

	void RenderScene::RemoveRenderCamera(RenderCamera* camera)
	{
		auto it = std::find_if(m_cameras.begin(), m_cameras.end(),
			[camera](const auto& p) { return p.get() == camera; });
		if (it != m_cameras.end())
			m_cameras.erase(it);
	}

	RenderCamera* RenderScene::GetMainCamera()
	{
		return m_cameras.empty() ? nullptr : m_cameras[0].get();
	}
}