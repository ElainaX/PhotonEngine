#pragma once

#include "RenderCamera.h"
#include "Light.h"
#include "RenderObject/RenderItem.h"
#include "RenderObject/RenderMeshCollection.h"

#include <memory>
#include <unordered_map>

namespace photon 
{

	class RenderScene
	{
	public:
		RenderScene(std::shared_ptr<RenderCamera> mainCamera)
		{
			Initialize(mainCamera);
		}

		void Initialize(std::shared_ptr<RenderCamera> mainCamera);

		CommonRenderItem* AddCommonRenderItem(std::shared_ptr<Mesh> mesh, Material* mat, Shader* shader, RenderLayer renderLayer, StaticModelObjectConstants objConstants);
		Light* AddDirectionalLight(Vector3 strength, Vector3 dir);
		Light* AddPointLight(Vector3 strength, Vector3 position, Vector3 dir, float falloffStart, float falloffEnd);
		Light* AddSpotLight(Vector3 strength, Vector3 position, Vector3 dir, float falloffStart, float falloffEnd, float spotPower);

		void SetMainRenderCamera(RenderCamera* mainCam);
		RenderCamera* AddRenderCamera(std::shared_ptr<RenderCamera> camera, bool bSetMainCamera = false);
		void RemoveRenderCamera(RenderCamera* camera);

		RenderCamera* GetMainCamera();
		std::vector<CommonRenderItem*> GetCommonRenderItems(RHI* rhi);



		std::vector<Light> directionalLights;
		std::vector<Light> pointLights;
		std::vector<Light> spotLights;


	private:

		std::shared_ptr<RenderMeshCollection> m_MeshCollection;
		std::vector<std::shared_ptr<CommonRenderItem>> m_RenderItems;
		std::vector<std::shared_ptr<RenderCamera>> m_Cameras;

	};


}