#pragma once

#include "RenderCamera.h"
#include "Light.h"
#include "RenderItem.h"

#include <memory>
#include <vector>

namespace photon
{
	class RenderScene
	{
	public:
		explicit RenderScene(std::shared_ptr<RenderCamera> mainCamera)
		{
			Initialize(mainCamera);
		}

		void Initialize(std::shared_ptr<RenderCamera> mainCamera);

		RenderItem* CreateRenderItem();

		std::vector<RenderItem*> GatherMutableRenderItems();
		std::vector<const RenderItem*> GatherRenderItems() const;

		DirLight* AddDirectionalLight(Vector3 strength, Vector3 dir);
		PointLight* AddPointLight(Vector3 strength, Vector3 position, float falloffStart, float falloffEnd);
		SpotLight* AddSpotLight(Vector3 strength, Vector3 position, Vector3 dir, float falloffStart, float falloffEnd, float spotPower);

		void SetCubemap(TextureHandle cubemap) { m_skybox = cubemap; }
		TextureHandle GetCubemap() const { return m_skybox; }

		void SetMainRenderCamera(RenderCamera* mainCam);
		RenderCamera* AddRenderCamera(std::shared_ptr<RenderCamera> camera, bool setMain = false);
		void RemoveRenderCamera(RenderCamera* camera);
		RenderCamera* GetMainCamera();

	public:
		std::vector<DirLight> directionalLights;
		std::vector<PointLight> pointLights;
		std::vector<SpotLight> spotLights;

		TextureHandle m_skybox = {};

	private:
		uint64_t m_nextRenderItemId = 1;
		std::vector<std::unique_ptr<RenderItem>> m_renderItems;
		std::vector<std::shared_ptr<RenderCamera>> m_cameras;
	};
}